// include the Direct3D Library file
#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <fstream>
#include <iostream>
#include <string>
#include "core/ID3DContextProvider.h"
#include <IGame.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

class Engine : public ID3DContextProvider {
public:
	Engine(HWND hWnd, std::weak_ptr<IGame> game);
	~Engine();

	static Engine *Get() { return _instance; }

	ID3D11DeviceContext1 *getD3DContext() { return context;  };
	ID3D11Device1 *getD3DDevice() { return dev;  };

	void render();

private:
	void _initDirectX();
	void _initPipeline();

private:
	HWND hWnd;
	IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
	ID3D11Device1 *dev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext1 *context;          // the pointer to our Direct3D device context
	ID3D11RenderTargetView *backbuffer;
	
	ID3D11InputLayout *pLayout;            // the pointer to the input layout

	static Engine *_instance;
	std::weak_ptr<IGame> _game;
};