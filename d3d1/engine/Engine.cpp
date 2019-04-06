#include "Engine.h"
#include "render/shader/Shader.h"
#include "system/Logging.h"
#include "render/buffer/D3DBuffer.h"
#include "render/buffer/D3DMemoryBuffer.h"
#include "render/renderer/SceneRenderer.h"
#include "render/shader/ShaderGenerator.h"
#include "utils/MeshGeneration.h"
#include "render/mesh/Mesh.h"
#include "DXCaps.h"
#include "loader/TextureLoader.h"
#include "render/texture/Texture.h"
#include "render/renderer/InputLayoutCache.h"
#include "system/Window.h"
#include "system/Input.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "utils/Performance.h"

Engine *Engine::_instance = nullptr;

//tbb::task_scheduler_init init(1);

Engine::Engine(HINSTANCE hInstance, uint32_t width, uint32_t height, std::weak_ptr<IGame> game) : _game(game) {
	_instance = this;
	SetThreadAffinityMask(GetCurrentThread(), 1 << 0);
	
	engine::Performance::initialize();

	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	_frequency = double(li.QuadPart);

	ENGLogSetOutputFile("log.txt");

	_input = std::make_unique<Input>();
	_window = std::make_unique<Window>(_input.get());

	_shaderGenerator = std::make_unique<ShaderGenerator>(); // before dx is ready
	hWnd = _window->initWindow(width, height, hInstance);

	_initDirectX();
	_sceneRenderer = std::make_unique<SceneRenderer>(); // after dx is ready

	QueryPerformanceCounter(&_lastTime);
}

Engine::~Engine()
{
	swapchain->Release();
	dev->Release();
	backbuffer->Release();
	context->Release();
}

void Engine::_initDirectX()
{
	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	// clear out the struct for use
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = _window->width();
	swapChainDesc.BufferDesc.Height = _window->height();
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapchain,
		(ID3D11Device **)&dev,
		NULL,
		(ID3D11DeviceContext **)&context);

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	// use the back buffer address to create the render target
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();


	ID3D11Texture2D* pDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = _window->width();
	descDepth.Height = _window->height();
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	
	ThrowIfFailed(dev->CreateTexture2D(&descDepth, NULL, &pDepthStencil));

	// Create the depth stencil view
	ThrowIfFailed(dev->CreateDepthStencilView(pDepthStencil, // Depth stencil texture
		NULL,//&descDSV, // Depth stencil desc
		&depthStencil));  // [out] Depth stencil view

	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	ThrowIfFailed(dev->CreateDepthStencilState(&dsDesc, &dsState));

	// Bind depth stencil state
	context->OMSetDepthStencilState(dsState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;


	// set the render target as the back buffer
	context->OMSetRenderTargets(1, &backbuffer, depthStencil);

	_initPipeline();
}

void Engine::startLoop() {
	while (!_window->quitTriggered()) {
		_window->processMessages();
		render();
	}
}

void Engine::_mainLoop() {
	
}


void Engine::_initPipeline() {

}

void Engine::render() {
	engine::Performance::startTimer(engine::Performance::Entry::Frame);

	if (!_initialized) {
		_initialized = true;
		_game.lock()->init();
	}

	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	double dt = double(time.QuadPart - _lastTime.QuadPart) / double(_frequency);
	_lastTime = time;
	_engineTime = time.QuadPart / double(_frequency);

	const float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	// clear the back buffer to a deep blue
	//context->ClearRenderTargetView(backbuffer, color);
	//context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = _window->width();
	viewport.Height = _window->height();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	_game.lock()->update(float(dt));
	engine::Performance::stopTimer(engine::Performance::Entry::Frame);

	engine::Performance::startTimer(engine::Performance::Entry::SwapBuffers);
	context->OMSetRenderTargets(1, &backbuffer, depthStencil);
	swapchain->Present(0, 0);
	engine::Performance::stopTimer(engine::Performance::Entry::SwapBuffers);
}

void Engine::renderScene(std::shared_ptr<Scene> scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D) {
	_sceneRenderer->renderScene(scene, camera, camera2D);
}