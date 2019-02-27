#include "SceneRenderer.h"
#include "Engine.h"
#include "DXCaps.h"
#include "scene/Scene.h"
#include "render/shader/ConstantBufferStruct.h"
#include "render/buffer/D3DMemoryBuffer.h"
#include "scene/GameObject.h"
#include "ConstantBufferManager.h"
#include "render/shader/ShaderGenerator.h"
#include "InputLayoutCache.h"

SceneRenderer::SceneRenderer() {
	_constantBufferManager = std::make_unique<ConstantBufferManager>();
	_inputLayoutCache = std::make_unique<InputLayoutCache>();
	//_inputLayoutCache = new InputLayoutCache();
}

void SceneRenderer::renderScene(ScenePtr scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D) {
	_clearQueues();

	auto visibleObjects = scene->visibleObjects(camera);
	for (auto &object : visibleObjects) {
		object->render(*this);
	}

	_prepareShaders();

	for (auto &queue : _queues) {
		for (auto &rop : queue) {
			if (rop.objectParams) {
				_constantBufferManager->setObjectParamsBlock(&rop);
			}
		}
	}

	_constantBufferManager->upload();

	for (auto &rop : _queues[(int)RenderQueue::Opaque]) {
		_setupROP(rop);
	}
}

void SceneRenderer::_clearQueues() {
	for (auto &queue : _queues) {
		queue.clear();
	}
}

void SceneRenderer::_prepareShaders() {
	auto engine = Engine::Get();
	for (auto cap : Material::_uninitializedCaps) {
		//engine->shaderGenerator()->getShaderWithCaps()
	}
}

void SceneRenderer::addRenderOperation(RenderOperation &rop, RenderQueue queue) {
	// TODO: CONCURRENCY

	_queues[(int)queue].push_back(rop);
	
	// Using friend's private functions
	if (rop.material && rop.material->_capsDirty) {
		rop.material->_updateCaps();
	}
}

void SceneRenderer::_setupROP(RenderOperation &rop) {
	auto context = Engine::Get()->getD3DContext();
	UINT stride = rop.mesh->strideBytes();
	UINT offset = 0;

	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

void SceneRenderer::renderMesh(MeshPtr mesh) {
	
}
