// include the Direct3D Library file
#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include <iostream>
#include <string>
#include "core/ID3DContextProvider.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

class Engine : public ID3DContextProvider {
public:
	Engine(HWND hWnd);
	~Engine();

	ID3D11DeviceContext *getD3DContext() { return context;  };
	ID3D11Device *getD3DDevice() { return dev;  };

	void render();

private:
	void _initDirectX();
	void _initPipeline();

private:
	HWND hWnd;
	IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
	ID3D11Device *dev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext *context;          // the pointer to our Direct3D device context
	ID3D11RenderTargetView *backbuffer;
	
	ID3D11InputLayout *pLayout;            // the pointer to the input layout
};