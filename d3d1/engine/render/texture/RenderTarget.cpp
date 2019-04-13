#include "RenderTarget.h"
#include "Texture.h"
#include "Engine.h"

RenderTarget::RenderTarget(int32_t width, int32_t height, TexturePtr colorTexture, TexturePtr depthTexture, bool color, bool depth)
	: _width(width), _height(height), _hasColor(color), _hasDepth(depth), _colorTexture(colorTexture), _depthTexture(depthTexture)
{
	_recreateTextures();
}

RenderTarget::RenderTarget(int32_t width, int32_t height, bool color, bool depth) 
	: RenderTarget(width, height, nullptr, nullptr, color, depth)
{
	
}

void RenderTarget::_recreateTextures()
{
	auto device = Engine::Get()->getD3DDevice();

	if (_hasColor && !_colorTexture) {
		_colorTexture = std::make_shared<Texture>();
		_colorTexture->initTexture2D(_width, _height, _colorFormat, nullptr, false, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	}

	if (_hasColor) {
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = _colorFormat;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;
		SafeRelease(&_renderTargetView);
		ThrowIfFailed(
			device->CreateRenderTargetView(_colorTexture->texture(), &renderTargetViewDesc, &_renderTargetView)
		);
	}

	if (_hasDepth && !_depthTexture) {
		_depthTexture = std::make_shared<Texture>();
		_depthTexture->initTexture2D(_width, _height, _depthFormat, nullptr, false, D3D11_BIND_DEPTH_STENCIL);
	}

	if (_depthTexture) {
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = _depthFormat;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		SafeRelease(&_depthStencilView);
		ThrowIfFailed(
			device->CreateDepthStencilView(_depthTexture->texture(), &depthStencilViewDesc, &_depthStencilView)
		);
	}
}

void RenderTarget::activate(ID3D11DeviceContext1 *context)
{
	UINT number = _hasColor ? 1 : 0;
	ID3D11RenderTargetView *colorTarget = _hasColor ? _renderTargetView : nullptr;
	context->OMSetRenderTargets(number, &colorTarget, _depthStencilView);
}
