#pragma once

#include <d3d11_1.h>
#include "Engine.h"
#include "IRenderer.h"	
#include "ICameraParamsProvider.h"
#include "RenderOperation.h"
#include "EngineTypes.h"

class PassConstantBufferManager;
class InputLayoutCache;
class IShadowCaster;
class RenderState;
class LightGrid;

class PassRenderer : public IRenderer {
public:
	PassRenderer(RenderTargetPtr renderTarget, std::shared_ptr<LightGrid> lightGrid, RenderMode mode, std::shared_ptr<InputLayoutCache> inputLayoutCache);

	ID3D11CommandList* commandList() const { return _commandList; }

	uvec2 size() const { return this->_camera ? this->_camera->cameraViewSize() : uvec2(0, 0); }

	std::shared_ptr<ICameraParamsProvider> camera() const { return _camera; }
	void camera(std::shared_ptr<ICameraParamsProvider> value) { _camera = value; }
	void setProjectorTexture(TexturePtr texture);
	void setShadowmapTexture(TexturePtr texture);

	void clearColor(bool value) { _clearColor = value; }
	bool clearColor() const { return _clearColor; }
	void clearDepth(bool value) { _clearDepth = value; }
	bool clearDepth() const { return _clearDepth; }

	RenderMode mode() const { return _mode; }
	void mode(RenderMode value) { _mode = value; }

	void render(ScenePtr scene, ICameraParamsProviderPtr camera);
	void addRenderOperation(RenderOperation &rop, RenderQueue queue) override;
	void renderMesh(MeshPtr mesh) override;

private:
	void _renderRop(RenderOperation &rop, RenderMode mode);
	void _renderQueues(RenderMode mode);
	void _clearQueues();

private:
	bool _clearDepth = true;
	bool _clearColor = true;
	RenderMode _mode = RenderMode::Normal;
	std::shared_ptr<ICameraParamsProvider> _camera;
	std::vector<RenderOperation> _queues[(int)RenderQueue::Count];
	std::vector<RenderOperation *> _skinningRops;
	mutable std::vector<std::shared_ptr<IShadowCaster>> _shadowCasters;

private:
	std::shared_ptr<LightGrid> _lightGrid;
	std::unique_ptr<PassConstantBufferManager> _constantBufferManager;
	std::unique_ptr<RenderState> _renderState;
	uint32_t _texture0Index;;
	uint32_t _projectorTextureIndex;
	uint32_t _shadowmapTextureIndex;
	RenderTargetPtr _renderTarget;
	std::shared_ptr<InputLayoutCache> _inputLayoutCache;
	ID3D11DeviceContext1 *_deferredContext;
	ID3D11CommandList* _commandList = nullptr;
};