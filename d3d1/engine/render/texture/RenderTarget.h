#pragma once

#include "EngineTypes.h"
#include <d3d11_1.h>

class RenderTarget {
public:
	enum class Mode : int {
		Color = 1 << 0,
		Depth = 1 << 1,
		ColorBindShaderResource = 1 << 2,
		DepthBindShaderResource = 1 << 3
	};

	RenderTarget(int32_t width, int32_t height, TexturePtr colorTexture, TexturePtr depthTexture, int mode, uint32_t sampleCount);
	RenderTarget(int32_t width, int32_t height, int mode, uint32_t sampleCount);
	~RenderTarget();

	ID3D11RenderTargetView *renderTargetView() const { return _renderTargetView; };
	ID3D11DepthStencilView *depthStencilView() const { return _depthStencilView; };

	TexturePtr framebufferTexture() const { return _colorTexture; }
	TexturePtr depthTexture() const { return _depthTexture; }

	void activate(ID3D11DeviceContext1 *context, bool color = true, bool depth = true);

private:
	void _recreateTextures();

private:
	uint32_t _sampleCount;
	int32_t _width;
	int32_t _height;
	TexturePtr _colorTexture;
	TexturePtr _depthTexture;
	ID3D11DepthStencilView *_depthStencilView = 0;
	bool _hasColor;
	bool _colorShaderResource;
	bool _hasDepth;
	bool _depthShaderResource;
	DXGI_FORMAT _colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT _depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT _depthShaderResourceFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DXGI_FORMAT _depthStencilViewFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ID3D11RenderTargetView *_renderTargetView = 0;
};