
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

Engine *Engine::_instance = nullptr;

Engine::Engine(HINSTANCE hInstance, uint32_t width, uint32_t height, std::weak_ptr<IGame> game) : _game(game) {
	_instance = this;
		
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
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

	// create a device, device context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
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

	// set the render target as the back buffer
	context->OMSetRenderTargets(1, &backbuffer, NULL);

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
	if (!_initialized) {
		_initialized = true;
		_game.lock()->init();
	}

	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	double dt = double(time.QuadPart - _lastTime.QuadPart) / double(_frequency);
	_lastTime = time;

	const float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	// clear the back buffer to a deep blue
	context->ClearRenderTargetView(backbuffer, color);

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = _window->width();
	viewport.Height = _window->height();

	context->RSSetViewports(1, &viewport);

	_game.lock()->update(float(dt));

	swapchain->Present(0, 0);
}

void Engine::renderScene(std::shared_ptr<Scene> scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D) {
	_sceneRenderer->renderScene(scene, camera, camera2D);
}