#include "SceneRenderer.h"
#include "Engine.h"
#include "DXCaps.h"
#include "scene/Scene.h"
#include "render/shader/ConstantBufferStruct.h"
#include "render/buffer/D3DMemoryBuffer.h"
#include "scene/GameObject.h"

SceneRenderer::SceneRenderer() {
	_objectParams = std::make_unique<D3DMemoryBuffer>(Engine::Get(), 
		D3D11_BIND_CONSTANT_BUFFER, 
		D3D11_USAGE_DYNAMIC, 
		DXCaps::CONSTANT_BUFFER_MAX_SIZE
	);
	_cameraData = std::make_unique<D3DMemoryBuffer>(Engine::Get(),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE_DYNAMIC,
		DXCaps::CONSTANT_BUFFER_MAX_SIZE
	);
}

void SceneRenderer::renderScene(ScenePtr scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D)
{
	_clearQueues();

	auto visibleObjects = scene->visibleObjects(camera);
	for (auto &object : visibleObjects) {
		object->render(*this);
	}

	for (auto &queue : _queues) {
		for (auto &rop : queue) {
			if (rop.objectParams) {
				auto address = _objectParams->appendData(
					(void *)rop.objectParams, sizeof(ConstantBufferStruct::ObjectParams), DXCaps::CONSTANT_BUFFER_ALIGNMENT
				);
				rop.objectParamsBlockOffset.index = 0;
				rop.objectParamsBlockOffset.offset = address;
			}
		}
	}

	_objectParams->upload();

	for (auto &rop : _queues[(int)RenderQueue::Opaque]) {
		_setupROP(rop);
	}
}

void SceneRenderer::_clearQueues() {
	for (auto &queue : _queues) {
		queue.clear();
	}
}

void SceneRenderer::addRenderOperation(RenderOperation &rop, RenderQueue queue)
{
	_queues[(int)queue].push_back(rop);
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
