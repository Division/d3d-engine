#pragma once

#include "EngineTypes.h"
#include "IRenderer.h"
#include "RenderOperation.h"

class D3DMemoryBuffer;
class ConstantBufferManager;
class InputLayoutCache;
class IShadowCaster;
class PassRenderer;
class ShadowMap;
class LightGrid;

class SceneRenderer {
public:
	SceneRenderer();
	virtual ~SceneRenderer() = default;

	void renderScene(ScenePtr scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D = nullptr);

	const InputLayoutCache *inputLayoutCache() const { return _inputLayoutCache.get(); }

private:
	void _clearQueues();
	void _prepareShaders();

private:
	std::unique_ptr<PassRenderer> _mainCameraRenderer;
	std::unique_ptr<PassRenderer> _depthPrePassRenderer;
	std::unique_ptr<ShadowMap> _shadowMap;
	std::shared_ptr<LightGrid> _lightGrid;
	std::shared_ptr<ConstantBufferManager> _constantBufferManager;
	std::shared_ptr<InputLayoutCache> _inputLayoutCache;
	//InputLayoutCache *_inputLayoutCache;
	std::vector<RenderOperation> _queues[(int)RenderQueue::Count];
	std::vector<RenderOperation *>_skinningRops;

	mutable std::vector<std::shared_ptr<IShadowCaster>> _shadowCasters;
};

