
#include "Engine.h"
#include "render/shader/Shader.h"
#include "system/Logging.h"
#include "render/buffer/D3DBuffer.h"
#include "render/buffer/D3DMemoryBuffer.h"
#include "render/renderer/SceneRenderer.h"
#include "utils/MeshGeneration.h"
#include "render/mesh/Mesh.h"
#include "DXCaps.h"
#include "loader/TextureLoader.h"
#include "render/texture/Texture.h"

std::shared_ptr<Shader> shader1;
std::shared_ptr<D3DBuffer> buffer1;
std::shared_ptr<D3DMemoryBuffer> constantBuffer1;
std::shared_ptr<Mesh> mesh;
std::shared_ptr<Texture> texture1;

float rotationAngle = 0;
Engine *Engine::_instance = nullptr;

struct ConstantBuffer {
	mat4 modelMatrix;
	//mat4 projectionMatrix;
};

struct VERTEX {
	FLOAT X, Y, Z;
	FLOAT Color[4];
};

Engine::Engine(HWND hWnd, std::weak_ptr<IGame> game) : hWnd(hWnd), _game(game) {
	_instance = this;

	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	_frequency = double(li.QuadPart);

	ENGLogSetOutputFile("log.txt");
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
	pLayout->Release();
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

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;
	viewport.Height = 600;

	context->RSSetViewports(1, &viewport);

	_initPipeline();
}

void Engine::_initPipeline() {
	
	// create a triangle using the VERTEX struct
	VERTEX OurVertices[] =
	{
		{0.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
		{0.45f, -0.5, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
		{-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}
	};

	buffer1 = std::make_shared<D3DBuffer>(this, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, sizeof(VERTEX) * 3, OurVertices);
	
	shader1 = std::make_shared<Shader>(this);
	shader1->loadFromFile("shader.hlsl.txt");

	constantBuffer1 = std::make_shared<D3DMemoryBuffer>(this, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, DXCaps::CONSTANT_BUFFER_MAX_SIZE);

	mesh = std::shared_ptr<Mesh>(new Mesh());
	MeshGeneration::generateQuad(mesh, vec2(1, 1));
	mesh->createBuffer();

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, mesh->vertexOffsetBytes(), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, mesh->texCoordOffsetBytes(), D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	pLayout = shader1->createInputLayout(ied, 2);
	context->IASetInputLayout(pLayout);

	texture1 = loader::loadTexture("resources/lama.png", false);
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

	_game.lock()->update(float(dt));
	// do 3D rendering on the back buffer here

	// select which vertex buffer to display
	UINT stride = mesh->strideBytes();
	UINT offset = 0;
	rotationAngle += dt * M_PI * 2;
	mat4 matrix;
	matrix = glm::rotate(matrix, rotationAngle, vec3(0, 0, 1));
	matrix = glm::transpose(matrix);

	// Shader data
	ConstantBuffer constantData = { matrix };
	constantBuffer1->appendData(&constantData, sizeof(ConstantBuffer), DXCaps::CONSTANT_BUFFER_ALIGNMENT);
	constantBuffer1->upload();
	auto constantBuffer = constantBuffer1->buffer();
	UINT firstConstant = 0;
	UINT constantCount = max((int)ceilf((float)sizeof(ConstantBuffer) / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE), DXCaps::CONSTANT_BUFFER_ALIGNMENT / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE);
	context->VSSetConstantBuffers1(
		0,
		1,
		&constantBuffer,
		&firstConstant,
		&constantCount
	);

	// Texture
	context->PSSetShaderResources(0, 1, texture1->resourcePointer());
	context->PSSetSamplers(0, 1, texture1->samplerStatePointer());

	// Buffers
	const auto buffer = mesh->vertexBuffer()->buffer();
	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->Draw(mesh->vertexCount(), 0);

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}

void Engine::renderScene(std::shared_ptr<Scene> scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D) {
	_sceneRenderer->renderScene(scene, camera, camera2D);
}