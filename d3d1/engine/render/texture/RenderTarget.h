#pragma once

#include "EngineTypes.h"
#include <d3d11_1.h>

class RenderTarget {
public:
	RenderTarget(int32_t width, int32_t height, TexturePtr colorTexture, TexturePtr depthTexture, bool color, bool depth);
	RenderTarget(int32_t width, int32_t height, bool color, bool depth);

	ID3D11RenderTargetView *renderTargetView() const { return _renderTargetView; };
	ID3D11DepthStencilView *depthStencilView() const { return _depthStencilView; };

	TexturePtr framebufferTexture() const { return _colorTexture; }
	TexturePtr depthTexture() const { return _depthTexture; }

	void activate(ID3D11DeviceContext1 *context, bool color = true, bool depth = true);

private:
	void _recreateTextures();

private:
	int32_t _width;
	int32_t _height;
	TexturePtr _colorTexture;
	TexturePtr _depthTexture;
	ID3D11DepthStencilView *_depthStencilView = 0;
	bool _hasColor;
	bool _hasDepth;
	DXGI_FORMAT _colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ID3D11RenderTargetView *_renderTargetView = 0;
};