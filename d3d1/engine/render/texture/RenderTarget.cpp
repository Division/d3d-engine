#include "RenderTarget.h"
#include "Texture.h"
#include "Engine.h"

RenderTarget::RenderTarget(int32_t width, int32_t height, TexturePtr colorTexture, TexturePtr depthTexture, int mode, uint32_t sampleCount)
	: _width(width), _height(height), _sampleCount(sampleCount),
	_hasColor(mode & (int)Mode::Color), _hasDepth(mode & (int)Mode::Depth), 
	_colorShaderResource(mode & (int)Mode::ColorBindShaderResource), _depthShaderResource(mode & (int)Mode::DepthBindShaderResource),
	_colorTexture(colorTexture), _depthTexture(depthTexture)
{
	if (_depthShaderResource) {
		_depthFormat = DXGI_FORMAT_R32_TYPELESS;
		_depthShaderResourceFormat = DXGI_FORMAT_R32_FLOAT;
		_depthStencilViewFormat = DXGI_FORMAT_D32_FLOAT;
	}
	_recreateTextures();
}

RenderTarget::RenderTarget(int32_t width, int32_t height, int mode, uint32_t sampleCount)
	: RenderTarget(width, height, nullptr, nullptr, mode, sampleCount) {
}

RenderTarget::RenderTarget(const RenderTargetInitializer &initializer) {
	_sampleCount = initializer.sampleCount;
	_width = initializer.width;
	_height = initializer.height;
	_colorTexture = initializer.colorTexture;
	_depthTexture = initializer.depthTexture;
	_hasColor = initializer.hasColor;
	_colorShaderResource = initializer.colorShaderResource;
	_hasDepth = initializer.hasDepth;
	_depthShaderResource = initializer.depthShaderResource;
	_colorFormat = initializer.colorFormat;
	_depthFormat = initializer.depthFormat;
	_depthShaderResourceFormat = _depthFormat;
	_depthStencilViewFormat = _depthFormat;
	
	if (_depthShaderResource) {
		_depthFormat = DXGI_FORMAT_R32_TYPELESS;
		_depthShaderResourceFormat = DXGI_FORMAT_R32_FLOAT;
		_depthStencilViewFormat = DXGI_FORMAT_D32_FLOAT;
	}

	_recreateTextures();
}

RenderTarget::~RenderTarget() {
	SafeRelease(&_renderTargetView);
	SafeRelease(&_depthStencilView);
}

void RenderTarget::_recreateTextures()
{
	auto device = Engine::Get()->getD3DDevice();
	bool multisampling = _sampleCount > 1;

	if (_hasColor && !_colorTexture) {
		_colorTexture = std::make_shared<Texture>();
		UINT bindFlags = D3D11_BIND_RENDER_TARGET;
		if (_colorShaderResource) {
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		_colorTexture->initTexture2D(_width, _height, _colorFormat, nullptr, false, bindFlags, DXGI_FORMAT_UNKNOWN, _sampleCount);
	}

	if (_hasColor) {
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = _colorFormat;	
		renderTargetViewDesc.ViewDimension = multisampling ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		SafeRelease(&_renderTargetView);
		ThrowIfFailed(
			device->CreateRenderTargetView(_colorTexture->texture(), &renderTargetViewDesc, &_renderTargetView)
		);
	}

	if (_hasDepth && !_depthTexture) {
		_depthTexture = std::make_shared<Texture>();
		UINT bindFlags = D3D11_BIND_DEPTH_STENCIL;
		if (_depthShaderResource) {
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		_depthTexture->initTexture2D(_width, _height, _depthFormat, nullptr, false, bindFlags, _depthShaderResourceFormat, _sampleCount);
	}

	if (_depthTexture) {
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = _depthStencilViewFormat;
		depthStencilViewDesc.ViewDimension = multisampling ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		SafeRelease(&_depthStencilView);
		ThrowIfFailed(
			device->CreateDepthStencilView(_depthTexture->texture(), &depthStencilViewDesc, &_depthStencilView)
		);
	}
}

void RenderTarget::activate(ID3D11DeviceContext1 *context, bool color, bool depth)
{
	bool hasColor = color && _hasColor;
	bool hasDepth = depth && _hasDepth;
	UINT number = hasColor ? 1 : 0;
	ID3D11RenderTargetView *colorTarget = hasColor ? _renderTargetView : nullptr;
	ID3D11DepthStencilView *depthStencilView = hasDepth ? _depthStencilView : nullptr;
	context->OMSetRenderTargets(number, &colorTarget, depthStencilView);
}
