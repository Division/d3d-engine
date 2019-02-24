#pragma once

#include "EngineTypes.h"
#include "IRenderer.h"
#include "RenderOperation.h"

class D3DMemoryBuffer;

class SceneRenderer : public IRenderer {
public:
	SceneRenderer();
	void renderScene(ScenePtr scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D = nullptr);

	// IRenderer
	void addRenderOperation(RenderOperation &rop, RenderQueue queue) override;
	void renderMesh(MeshPtr mesh) override;

private:
	void _setupROP(RenderOperation &rop);
	void _clearQueues();

private:
	std::unique_ptr<D3DMemoryBuffer> _objectParams;
	std::unique_ptr<D3DMemoryBuffer> _cameraData;

	std::vector<RenderOperation> _queues[(int)RenderQueue::Count];
};

