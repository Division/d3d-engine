
#include "Engine.h"
#include "render/Shader.h"
#include "system/Logging.h"
#include "render/buffer/D3DBuffer.h"

std::shared_ptr<Shader> shader1;
std::shared_ptr<D3DBuffer> buffer1;

Engine *Engine::_instance = nullptr;

struct VERTEX {
	FLOAT X, Y, Z;
	FLOAT Color[4];
};

Engine::Engine(HWND hWnd, std::weak_ptr<IGame> game) : hWnd(hWnd), _game(game) {
	_instance = this;
	ENGLogSetOutputFile("log.txt");
	_initDirectX();
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

	// create the input layout object
	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	pLayout = shader1->createInputLayout(ied, 2);
	context->IASetInputLayout(pLayout);
}

void Engine::render() {
	const float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
	// clear the back buffer to a deep blue
	context->ClearRenderTargetView(backbuffer, color);

	// do 3D rendering on the back buffer here

	// select which vertex buffer to display
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	const auto buffer = buffer1->buffer();

	context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);

	// select which primtive type we are using
	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// draw the vertex buffer to the back buffer
	context->Draw(3, 0);

	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);
}