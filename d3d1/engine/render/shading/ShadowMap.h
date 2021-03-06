//
// Created by Sidorenko Nikita on 2018-11-28.
//

#ifndef CPPWRAPPER_SHADOWMAP_H
#define CPPWRAPPER_SHADOWMAP_H

#include <memory>
#include "EngineTypes.h"
#include "EngineMath.h"
#include "render/shading/IShadowCaster.h"
#include <vector>

class Renderer;
class InputLayoutCache;

class ShadowMap {
public:
	ShadowMap(unsigned int resolutionX, unsigned int resolutionY, std::shared_ptr<InputLayoutCache> inputLayoutCache);

	void setupRenderPasses(const std::vector<IShadowCasterPtr> &shadowCasters);
	void renderShadowMaps(const std::vector<IShadowCasterPtr> &shadowCasters, const ScenePtr &scene);
	PassRendererPtr *renderPasses() const { return const_cast<PassRendererPtr *>(_renderers.data()); }
	int32_t renderPassCount() const { return _shadowCasterCount; }
	void execute(ID3D11DeviceContext *immediateContext) const;
	
  TexturePtr depthAtlas() const;
private:
  uvec2 _resolution;
  uvec2 _cellPixelSize;
  vec2 _cellSize;
  unsigned int _pixelSpacing = 2;
  RenderTargetPtr _depthAtlas;
  std::vector<PassRendererPtr> _renderers;
  std::shared_ptr<InputLayoutCache> _inputLayoutCache;
  uint32_t _shadowmapBlock;
  int32_t _shadowCasterCount;

private:
  Rect getCellPixelRect(unsigned int index);
  Rect getCellRect(unsigned int index);
};


#endif //CPPWRAPPER_SHADOWMAP_H
