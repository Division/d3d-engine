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
#include "render/material/Material.h"
#include "render/shading/IShadowCaster.h"
#include "objects/LightObject.h"
#include "objects/Projector.h"
#include "render/texture/Texture.h"

SceneRenderer::SceneRenderer() {
	_constantBufferManager = std::make_unique<ConstantBufferManager>();
	_inputLayoutCache = std::make_unique<InputLayoutCache>();
}

void SceneRenderer::renderScene(ScenePtr scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D) {
	if (!camera) { return; }
	_clearQueues();

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

	_prepareShaders();
	_skinningRops.clear();

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
	auto shader = Engine::Get()->shaderGenerator()->getShaderWithCaps(caps);
	auto layout = _inputLayoutCache->getLayout(rop.mesh, shader);
	context->IASetInputLayout(layout);

	shader->bind();

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
