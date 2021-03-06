// include the Direct3D Library file
#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <fstream>
#include <iostream>
#include <string>
#include "core/ID3DContextProvider.h"
#include <IGame.h>
#include "EngineTypes.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

class SceneRenderer;
class Scene;
class ShaderGenerator;
class Window;
class Input;
class RenderTarget;

class Engine : public ID3DContextProvider {
public:
	Engine(HINSTANCE hInstance, uint32_t width, uint32_t height, uint sampleCount, std::weak_ptr<IGame> game);
	~Engine();

	static Engine *Get() { return _instance; }

	double time() const { return _engineTime; }
	void startLoop();

	ID3D11DeviceContext1 *getD3DContext() { return context;  };
	ID3D11Device1 *getD3DDevice() { return dev;  };
	const ShaderGenerator *shaderGenerator() const { return _shaderGenerator.get(); }
	const Input *input() const { return _input.get(); }
	const Window *window() const { return _window.get(); }

	RenderTargetPtr renderTarget() const { return _renderTarget; }
	void projectorTexture(TexturePtr texture);
	TexturePtr projectorTexture() const;
	uint32_t multisampleCount() const { return _sampleCount; }
	void render();
	void renderScene(std::shared_ptr<Scene> scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D);

private:
	void _mainLoop();
	bool _initDirectX();

private:
	HWND hWnd;
	IDXGISwapChain *swapchain;
	ID3D11Device1 *dev;
	ID3D11DeviceContext1 *context;     
	ID3D11Debug *_dxDebug = nullptr;
	//ID3D11RenderTargetView *backbuffer;
	//ID3D11DepthStencilView *depthStencil;
	ID3D11DepthStencilState *dsState;

	ID3D11InputLayout *pLayout;            

	std::unique_ptr<Input> _input;
	std::unique_ptr<Window> _window;
	std::unique_ptr<SceneRenderer> _sceneRenderer;
	std::unique_ptr<ShaderGenerator> _shaderGenerator;
	RenderTargetPtr _renderTarget;

	static Engine *_instance;
	std::weak_ptr<IGame> _game;

	uint32_t _sampleCount;
	bool _initialized = false; // Game interface initialized
	bool _engineInitialized = false; // DX render initialized
	LARGE_INTEGER _lastTime;
	double _engineTime;
	double _frequency = 0.0;
};

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception("Operation failed");
	}
}