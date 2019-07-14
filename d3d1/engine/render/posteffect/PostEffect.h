#pragma once

#include <memory>
#include "EngineMath.h"

class RenderTarget;
class Mesh;
class Texture;
class Shader;
class InputLayoutCache;

class PostEffect {
public:
	static const int RENDER_TARGET_COUNT = 2;

	PostEffect(uint32_t width, uint32_t height, std::shared_ptr<InputLayoutCache> inputLayoutCache);
	void render(ID3D11DeviceContext1 *context, std::shared_ptr<Texture> input, std::shared_ptr<RenderTarget> output);

private:
	void _recreateRenderTargets();
	void _drawFullScreenQuad(ID3D11DeviceContext1 *context);

private:
	int2 _viewportSize;
	std::shared_ptr<Shader> _shader;
	std::shared_ptr<Mesh> _fullScreenQuad;
	std::shared_ptr<RenderTarget> _renderTargets[PostEffect::RENDER_TARGET_COUNT];
	std::shared_ptr<RenderTarget> _hdrInput;
	std::shared_ptr<InputLayoutCache> _inputLayoutCache;
};