#pragma once

#include "EngineTypes.h"
#include <d3d11_1.h>

struct RenderTargetInitializer {

	RenderTargetInitializer &size(uint32_t width, uint32_t height) {
		this->width = width;
		this->height = height;
		return *this;
	}

	RenderTargetInitializer &colorTarget(bool bindShaderResource, uint32_t sampleCount = 1, DXGI_FORMAT colorFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, TexturePtr colorTexture = nullptr) {
		hasColor = true;
		colorShaderResource = bindShaderResource;
		this->sampleCount = sampleCount;
		this->colorFormat = colorFormat;
		this->colorTexture = colorTexture;
		return *this;
	}

	RenderTargetInitializer &depthTarget(bool bindShaderResource, DXGI_FORMAT depthFormat = DXGI_FORMAT_R32_TYPELESS, TexturePtr depthTexture = nullptr) {
		hasDepth = true;
		depthShaderResource = bindShaderResource;
		this->depthFormat = depthFormat;
		this->depthTexture = depthTexture;
		return *this;
	}

	uint32_t sampleCount = 1;
	uint32_t width = 0;
	uint32_t height = 0;
	TexturePtr colorTexture = nullptr;
	TexturePtr depthTexture = nullptr;
	bool hasColor = false;
	bool colorShaderResource = false;
	bool hasDepth = false;
	bool depthShaderResource = false;
	DXGI_FORMAT colorFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DXGI_FORMAT depthFormat = DXGI_FORMAT_R32_TYPELESS;
};

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
	RenderTarget(const RenderTargetInitializer &initializer);
	~RenderTarget();

	ID3D11RenderTargetView *renderTargetView() const { return _renderTargetView; };
	ID3D11DepthStencilView *depthStencilView() const { return _depthStencilView; };

	TexturePtr framebufferTexture() const { return _colorTexture; }
	TexturePtr depthTexture() const { return _depthTexture; }

	const int32_t width() const { return _width;  }
	const int32_t height() const { return _height; }

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
	DXGI_FORMAT _colorFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
	DXGI_FORMAT _depthFormat = DXGI_FORMAT_R32_TYPELESS;
	DXGI_FORMAT _depthShaderResourceFormat = DXGI_FORMAT_R32_TYPELESS;
	DXGI_FORMAT _depthStencilViewFormat = DXGI_FORMAT_R32_TYPELESS;
	ID3D11RenderTargetView *_renderTargetView = 0;
};