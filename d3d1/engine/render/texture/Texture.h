//
// Created by Sidorenko Nikita on 3/25/18.
//

#ifndef CPPWRAPPER_TEXTURE_H
#define CPPWRAPPER_TEXTURE_H

#include <d3d11_1.h>
#include <memory>

struct TextureParams {

};

class Texture {
public:
  Texture() = default;
  ~Texture();
  explicit Texture(ID3D11Texture2D *texture): _texture(texture) {}

  ID3D11Texture2D *texture() const { return _texture; };
  ID3D11Texture2D **texturePointer() { return &_texture; };
  ID3D11Texture2D *const *texturePointer() const { return &_texture; };
  ID3D11ShaderResourceView *resource() const { return _textureView; };
  ID3D11ShaderResourceView *const *resourcePointer() const { return &_textureView; };
  ID3D11SamplerState *samplerState() const { return _samplerState; };
  ID3D11SamplerState *const *samplerStatePointer() const { return &_samplerState; };

  void initTexture2D(int32_t width, int32_t height, int32_t channels, bool sRGB, void *data, bool mipmaps = true);
  void initTexture2D(int width, int height, DXGI_FORMAT format, void *data, bool mipmaps, UINT bindFlags = D3D11_BIND_SHADER_RESOURCE);

private:
	ID3D11Texture2D *_texture = 0;
	ID3D11ShaderResourceView *_textureView = 0;
	ID3D11SamplerState *_samplerState = 0;

private:
	void _release();
	void _uploadData();
};

typedef std::shared_ptr<Texture> TexturePtr;

#endif //CPPWRAPPER_TEXTURE_H
