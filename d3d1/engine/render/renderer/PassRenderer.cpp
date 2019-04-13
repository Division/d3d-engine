#include "PassRenderer.h"
#include "Engine.h"
#include "ConstantBufferManager.h"
#include "InputLayoutCache.h"
#include "render/texture/RenderTarget.h"
#include "render/material/Material.h"
#include "render/texture/Texture.h"
#include "render/mesh/Mesh.h"
#include "render/buffer/D3DMemoryBuffer.h"
#include "render/shader/ShaderGenerator.h"
#include "render/shading/IShadowCaster.h"
#include "scene/Scene.h"
#include "objects/LightObject.h"
#include "objects/Projector.h"
#include "tbb/tbb.h"
#include "RenderState.h"

tbb::spin_mutex MaterialMutex;

PassRenderer::PassRenderer(RenderTargetPtr renderTarget, RenderMode mode, std::shared_ptr<InputLayoutCache> inputLayoutCache) :
						   _renderTarget(renderTarget), _inputLayoutCache(inputLayoutCache), _mode(mode) {

	auto device = Engine::Get()->getD3DDevice();
	device->CreateDeferredContext1(0, &_deferredContext);
	_constantBufferManager = std::make_unique<PassConstantBufferManager>(_deferredContext);
	_renderState = std::make_unique<RenderState>(_deferredContext);
}

void PassRenderer::_clearQueues() {
	for (auto &queue : _queues) {
		queue.clear();
	}
}

void PassRenderer::addRenderOperation(RenderOperation &rop, RenderQueue queue) {
	_queues[(int)queue].push_back(rop);

	// Using friend's private functions
	tbb::spin_mutex::scoped_lock lock(MaterialMutex);
	if (rop.material && rop.material->_capsDirty) {
		rop.material->_updateCaps();
	}
}

void PassRenderer::renderMesh(MeshPtr mesh) {

}

void PassRenderer::render(ScenePtr scene, ICameraParamsProviderPtr camera) {
	if (!camera) { return; }
	_clearQueues();

	auto cameraViewport = camera->cameraViewport();
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = cameraViewport.x;
	viewport.TopLeftY = cameraViewport.y;
	viewport.Width = cameraViewport.z;
	viewport.Height = cameraViewport.w;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	_deferredContext->RSSetViewports(1, &viewport);

	auto engine = Engine::Get();
	auto renderTarget = _renderTarget->renderTargetView();
	auto depthStencil = _renderTarget->depthStencilView();
	_renderTarget->activate(_deferredContext);

	if (_clearColor) {
		const float color[4] = { 1.0f, 0.2f, 0.4f, 1.0f };
		_deferredContext->ClearRenderTargetView(renderTarget, color);
	}
	if (_clearDepth) {
		_deferredContext->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	// Shadow casters
	auto &visibleLights = scene->visibleLights(camera);
	_shadowCasters.clear();
	for (auto &light : visibleLights) {
		if (light->castShadows()) {
			_shadowCasters.push_back(std::static_pointer_cast<IShadowCaster>(light));
		}
	}

	auto &visibleProjectors = scene->visibleProjectors(camera);
	for (auto &projector : visibleProjectors) {
		if (projector->castShadows()) {
			_shadowCasters.push_back(std::static_pointer_cast<IShadowCaster>(projector));
		}
	}

	_constantBufferManager->addCamera(std::static_pointer_cast<ICameraParamsProvider>(camera));

	//_shadowMap->setupShadowCasters(_shadowCasters);
	//_renderer->setupBuffers(scene, camera, camera2D);
	//_shadowMap->renderShadowMaps(_shadowCasters, scene);

	auto visibleObjects = scene->visibleObjects(camera);
	for (auto &object : visibleObjects) {
		object->render(*this);
	}

	for (auto &queue : _queues) {
		for (auto &rop : queue) {
			if (rop.objectParams) {
				_constantBufferManager->setObjectParamsBlock(&rop);
			}
			if (rop.skinningMatrices) {
				_constantBufferManager->setSkinningMatricesBlock(&rop);
			}
		}
	}

	_constantBufferManager->upload();
	_constantBufferManager->activateCamera(camera->cameraIndex());
	_renderQueues(_mode);
}

void PassRenderer::_renderQueues(RenderMode mode) {
	_renderState->setRenderMode(mode);

	for (auto &rop : _queues[(int)RenderQueue::Opaque]) {
		_renderRop(rop, mode);
	}

	SafeRelease(&_commandList);
	ThrowIfFailed(_deferredContext->FinishCommandList(false, &_commandList));
}

void PassRenderer::_renderRop(RenderOperation &rop, RenderMode mode) {
	auto context = this->_deferredContext;
	UINT stride = rop.mesh->strideBytes();
	UINT offset = 0;

	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	_constantBufferManager->setObjectParams(rop.objectParamsBlockOffset);
	if (rop.skinningMatrices) {
		_constantBufferManager->setSkinningMatrices(rop.skinningOffset);
	}

	auto &material = rop.material;
	if (material->_capsDirty) {
		material->_updateCaps();
	}

	auto texture0 = material->texture0();
	if (texture0) {
		context->PSSetShaderResources(0, 1, texture0->resourcePointer());
		context->PSSetSamplers(0, 1, texture0->samplerStatePointer());
	}

	auto &caps = rop.skinningMatrices ? material->shaderCapsSkinning() : material->shaderCaps();
	auto shader = Engine::Get()->shaderGenerator()->getShaderWithCaps(caps); // TODO: concurrency
	auto layout = _inputLayoutCache->getLayout(rop.mesh, shader); // TODO: concurrency
	context->IASetInputLayout(layout);

	shader->bind(context);

	if (rop.mesh->hasVertices()) {
		auto vertexBuffer = rop.mesh->vertexBuffer()->buffer();
		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	}

	if (rop.mesh->hasIndices()) {
		auto indexBuffer = rop.mesh->indexBuffer()->buffer();
		context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
		context->DrawIndexed(rop.mesh->indexCount(), 0, 0);
	}
	else {
		context->Draw(rop.mesh->indexCount(), 0);
	}
}
