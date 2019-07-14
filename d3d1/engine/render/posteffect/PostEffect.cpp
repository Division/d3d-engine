#include "PostEffect.h"
#include "render/texture/RenderTarget.h"
#include "render/texture/Texture.h"
#include "utils/MeshGeneration.h"
#include "Engine.h"
#include "render/mesh/Mesh.h"
#include "render/buffer/D3DMemoryBuffer.h"
#include "render/shader/ShaderGenerator.h"
#include "render/shader/ShaderCaps.h"
#include "render/shader/Shader.h"
#include "render/renderer/InputLayoutCache.h"

PostEffect::PostEffect(uint32_t width, uint32_t height, std::shared_ptr<InputLayoutCache> inputLayoutCache)
	: _viewportSize(width, height), _inputLayoutCache(inputLayoutCache) {
	
	_fullScreenQuad = std::make_shared<Mesh>();
	MeshGeneration::generateFullScreenQuad(_fullScreenQuad);
	_fullScreenQuad->createBuffer();
	
	_shader = Engine::Get()->shaderGenerator()->getShaderWithCaps(
		ShaderCapsSet(), "resources/shaders/posteffect/fullScreenQuad.hlsl"
	);

	_recreateRenderTargets();
}

void PostEffect::_recreateRenderTargets() {
	auto renderTargetInit = RenderTargetInitializer()
		.size(_viewportSize.x, _viewportSize.y)
		.colorTarget(true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);

	for (int i = 0; i < PostEffect::RENDER_TARGET_COUNT; i++) {
		_renderTargets[i] = std::make_shared<RenderTarget>(renderTargetInit);
	}
}

void PostEffect::render(ID3D11DeviceContext1 *context, std::shared_ptr<Texture> input, std::shared_ptr<RenderTarget> output) {
	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	output->activate(context, true, false);

	auto& texture = input;
	if (texture) {
		context->PSSetShaderResources(0, 1, texture->resourcePointer());
		context->PSSetSamplers(0, 1, texture->samplerStatePointer());
	}

	const float color[4] = { 1.0f, 0.2f, 0.4f, 1.0f };
	context->ClearRenderTargetView(output->renderTargetView(), color);
	_shader->bind(context);

	auto layout = _inputLayoutCache->getLayout(_fullScreenQuad, _shader); // TODO: concurrency
	context->IASetInputLayout(layout);
	
	D3D11_VIEWPORT viewport = {0, 0, output->width(), output->height()};
	context->RSSetViewports(1, &viewport);

	_drawFullScreenQuad(context);
}


void PostEffect::_drawFullScreenQuad(ID3D11DeviceContext1 *context) {
	auto vertexBuffer = _fullScreenQuad->vertexBuffer()->buffer();
	UINT stride = _fullScreenQuad->strideBytes();
	UINT offset = 0;


	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->Draw(_fullScreenQuad->indexCount(), 0);
}

