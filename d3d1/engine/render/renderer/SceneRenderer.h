#pragma once

#include "EngineTypes.h"
#include "IRenderer.h"
#include "RenderOperation.h"

class D3DMemoryBuffer;
class ConstantBufferManager;
class InputLayoutCache;

class SceneRenderer : public IRenderer {
public:
	SceneRenderer();
	void renderScene(ScenePtr scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D = nullptr);

	const InputLayoutCache *inputLayoutCache() const { return _inputLayoutCache.get(); }

	// IRenderer
	void addRenderOperation(RenderOperation &rop, RenderQueue queue) override;
	void renderMesh(MeshPtr mesh) override;

private:
	void _setupROP(RenderOperation &rop);
	void _clearQueues();
	void _prepareShaders();

private:
	std::unique_ptr<ConstantBufferManager> _constantBufferManager;
	std::unique_ptr<InputLayoutCache> _inputLayoutCache;
	//InputLayoutCache *_inputLayoutCache;
	std::vector<RenderOperation> _queues[(int)RenderQueue::Count];
};

