//
// Created by Sidorenko Nikita on 3/25/18.
//

#include "Texture.h"
#include "system/Logging.h"
#include "Engine.h"

void Texture::_uploadData() {

}

void Texture::_release() {
	SafeRelease(&_texture);
	SafeRelease(&_textureView);
	SafeRelease(&_samplerState);
}

void Texture::initTexture2D(int32_t width, int32_t height, int32_t channels, bool sRGB, void *data, bool mipmaps) {

	DXGI_FORMAT format;
	format = sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	/*switch (channels) {
	  case 4:
		format = sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		break;

	  case 3:
		format = sRGB ? DXGI_FORMAT_R8G8B: DXGI_FORMAT_R8G8B8_UNORM;
		break;

	  default:
		throw std::runtime_error("Invalid texture channel number");
	}*/

	this->initTexture2D(width, height, format, data, mipmaps);
}

void Texture::initTexture2D(int width, int height, DXGI_FORMAT format, void *data, bool mipmaps, UINT bindFlags, DXGI_FORMAT shaderResourceFormat) {
	_release();

	// Texture
	D3D11_SUBRESOURCE_DATA textureSubresourceData;
	ZeroMemory(&textureSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	textureSubresourceData.pSysMem = data;
	textureSubresourceData.SysMemPitch = width * 4; // hardcode size for now

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Format = format;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.MipLevels = mipmaps ? 0 : 1;
	//textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;

	if (mipmaps) {
		bindFlags |= D3D11_BIND_RENDER_TARGET;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}

	// Don't use multi-sampling.
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;

	textureDesc.BindFlags = bindFlags;

	ThrowIfFailed(
		Engine::Get()->getD3DDevice()->CreateTexture2D(
			&textureDesc,
			nullptr,//data ? &textureSubresourceData : nullptr,
			&_texture
		)
	);

	if (data) {
		Engine::Get()->getD3DContext()->UpdateSubresource(_texture, 0, NULL, data, textureSubresourceData.SysMemPitch, textureDesc.Height * textureSubresourceData.SysMemPitch);
	}

	if (shaderResourceFormat == DXGI_FORMAT_UNKNOWN) {
		shaderResourceFormat = textureDesc.Format;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
	ZeroMemory(&textureViewDesc, sizeof(textureViewDesc));
	textureViewDesc.Format = shaderResourceFormat;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MipLevels = mipmaps ? -1 : 1;
	textureViewDesc.Texture2D.MostDetailedMip = 0;

	// Sampler
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	// The sampler does not use anisotropic filtering, so this parameter is ignored.
	samplerDesc.MaxAnisotropy = 0;

	// Specify how texture coordinates outside of the range 0..1 are resolved.
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	// Use no special MIP clamping or bias.
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Don't use a comparison function.
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	ThrowIfFailed(
		Engine::Get()->getD3DDevice()->CreateSamplerState(
			&samplerDesc,
			&_samplerState
		)
	);

	if (bindFlags & D3D11_BIND_SHADER_RESOURCE) {
		ThrowIfFailed(
			Engine::Get()->getD3DDevice()->CreateShaderResourceView(
				_texture,
				&textureViewDesc,
				&_textureView
			)
		);

		if (mipmaps) {
			Engine::Get()->getD3DContext()->GenerateMips(_textureView);
		}
	}

	ENGLog("TEXTURE 2D LOADED, %ix%i", width, height);
}

Texture::~Texture() {
	_release();
}
