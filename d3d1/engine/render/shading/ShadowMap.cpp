//
// Created by Sidorenko Nikita on 2018-11-28.
//

#include "Engine.h"
#include "ShadowMap.h"
#include "IShadowCaster.h"
#include "render/renderer/PassRenderer.h"
#include "render/shader/ShaderDefines.h"
#include "render/texture/RenderTarget.h"
#include "render/renderer/InputLayoutCache.h"
#include "tbb/tbb.h"
#include <d3d11_1.h>
#include "render/texture/Texture.h"

const unsigned int CELL_COUNT = 4;
const auto MAX_MAPS = CELL_COUNT * CELL_COUNT;

ShadowMap::ShadowMap(unsigned int resolutionX, unsigned int resolutionY, std::shared_ptr<InputLayoutCache> inputLayoutCache) {
  _resolution = uvec2(resolutionX, resolutionY);
  _inputLayoutCache = inputLayoutCache;
  _depthAtlas = std::make_shared<RenderTarget>(
	  resolutionX, resolutionY, 
	  (int)RenderTarget::Mode::Depth | (int)RenderTarget::Mode::DepthBindShaderResource
  );

  float emptySpacing = (float)((CELL_COUNT - 1u) * _pixelSpacing);
  _cellPixelSize = glm::floor(vec2(resolutionX - emptySpacing, resolutionY - emptySpacing) / (float)CELL_COUNT);
  _cellSize = vec2(_cellPixelSize) / vec2(_resolution);

  _shadowmapBlock = SHADER_SAMPLER_REGISTERS.at(ShaderResourceName::ShadowMap);
}

void ShadowMap::setupRenderPasses(const std::vector<IShadowCasterPtr> &shadowCasters) {
	unsigned int index = 0;

	for (auto &caster : shadowCasters) {
		if (index >= MAX_MAPS) {
			caster->viewport(vec4(0,0,0,0));
			continue;
		}

		vec4 viewport = (vec4)getCellPixelRect(index);
		caster->viewport(viewport);
		index++;
	}

	_shadowCasterCount = index;

	if (_renderers.size() < _shadowCasterCount) {
		for (int i = _renderers.size(); i < _shadowCasterCount; i++) {
			auto renderer = std::make_shared<PassRenderer>(
				_depthAtlas,
				nullptr,
				RenderMode::DepthOnly,
				_inputLayoutCache
			);
			renderer->clearColor(false);
			renderer->clearDepth(false);
			_renderers.push_back(renderer);
		}
	}

	_depthAtlas->activate(Engine::Get()->getD3DContext());
	Engine::Get()->getD3DContext()->ClearDepthStencilView(_depthAtlas->depthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMap::renderShadowMaps(const std::vector<IShadowCasterPtr> &shadowCasters, const ScenePtr &scene) {
	for (int i = 0; i < _shadowCasterCount; i++) {
		_renderers[i]->render(scene, shadowCasters[i]);
	}
}

void ShadowMap::execute(ID3D11DeviceContext *immediateContext) const
{
	for (int i = 0; i < _shadowCasterCount; i++) {
		auto commandList = _renderers[i]->commandList();
		if (commandList) {
			immediateContext->ExecuteCommandList(commandList, false);
		}
	}
}


Rect ShadowMap::getCellPixelRect(unsigned int index) {
  unsigned int x = index % CELL_COUNT * (_cellPixelSize.x + _pixelSpacing);
  unsigned int y = index / CELL_COUNT * (_cellPixelSize.y + _pixelSpacing);

  return Rect((float)x, (float)y, (float)_cellPixelSize.x, (float)_cellPixelSize.y);
}

Rect ShadowMap::getCellRect(unsigned int index) {
  int x = index % CELL_COUNT;
  int y = index / CELL_COUNT;

  vec2 origin = vec2(x, y) * (vec2(_cellPixelSize) + (float)_pixelSpacing) / vec2(_resolution);

  return Rect(origin.x, origin.y, _cellSize.x, _cellSize.y);
}

TexturePtr ShadowMap::depthAtlas() const {
  return _depthAtlas->depthTexture();
}
