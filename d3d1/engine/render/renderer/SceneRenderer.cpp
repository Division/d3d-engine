#include "SceneRenderer.h"
#include "Engine.h"
#include "DXCaps.h"
#include "scene/Scene.h"
#include "render/shader/ConstantBufferStruct.h"
#include "render/buffer/D3DMemoryBuffer.h"
#include "scene/GameObject.h"
#include "ConstantBufferManager.h"
#include "render/shader/ShaderGenerator.h"
#include "render/shading/ShadowMap.h"
#include "InputLayoutCache.h"
#include "render/material/Material.h"
#include "render/shading/IShadowCaster.h"
#include "objects/LightObject.h"
#include "objects/Projector.h"
#include "render/texture/Texture.h"
#include "PassRenderer.h"
#include "render/shading/LightGrid.h"
#include "tbb/tbb.h"
#include "cvmarkersobj.h"

Concurrency::diagnostic::marker_series markers_render(_T("render"));

const unsigned int SHADOW_ATLAS_SIZE = 4096;

SceneRenderer::SceneRenderer() {
	_constantBufferManager = std::make_shared<ConstantBufferManager>();
	_inputLayoutCache = std::make_shared<InputLayoutCache>();
	_shadowMap = std::make_unique<ShadowMap>(SHADOW_ATLAS_SIZE, SHADOW_ATLAS_SIZE, _inputLayoutCache);
	_lightGrid = std::make_shared<LightGrid>();
	_mainCameraRenderer = std::make_unique<PassRenderer>(Engine::Get()->renderTarget(), _lightGrid, RenderMode::Normal, _inputLayoutCache);
	_mainCameraRenderer->clearColor(true);
	_mainCameraRenderer->clearDepth(false);
	_depthPrePassRenderer = std::make_unique<PassRenderer>(Engine::Get()->renderTarget(), nullptr, RenderMode::DepthOnly, _inputLayoutCache);
	_depthPrePassRenderer->clearColor(false);
	_depthPrePassRenderer->clearDepth(true);
}

void SceneRenderer::renderScene(ScenePtr scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D) {
	if (!camera) { return; }
	auto context = Engine::Get()->getD3DContext();


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

	// Light grid setup
	auto windowSize = camera->cameraViewSize();
	_lightGrid->update(windowSize.x, windowSize.y);

	auto lights = scene->visibleLights(camera);
	_lightGrid->appendLights(lights, camera);
	auto projectors = scene->visibleProjectors(camera);
	_lightGrid->appendProjectors(projectors, camera);

	// TODO: better concurrency for frustum culling
	for (auto &shadowCaster : _shadowCasters) {
		scene->visibleObjects(shadowCaster);
	}

	_lightGrid->upload();
	//_lightGrid->bindBuffers();

	{
		Concurrency::diagnostic::span s1(markers_render, _T("Multiple thread rendering"));

		scene->visibleObjects(camera);

		tbb::task_group renderTaskGroup; // concurrently execute render passes

		_shadowMap->setupRenderPasses(_shadowCasters);
		// Shadow map passes
		auto renderPasses = _shadowMap->renderPasses();
		for (int i = 0; i < _shadowMap->renderPassCount(); i++) {
			auto &pass = renderPasses[i];
			auto &shadowCaster = _shadowCasters[i];
			renderTaskGroup.run([&] {
				pass->render(scene, shadowCaster);
			});
		}

		// Depth pre pass
		renderTaskGroup.run([&] {
			_depthPrePassRenderer->render(scene, camera);
		});

		// Main camera
		renderTaskGroup.run([&] {
			_mainCameraRenderer->render(scene, camera);
		});

		renderTaskGroup.wait(); // wait until all render passes completed
	}

	Concurrency::diagnostic::span s1(markers_render, _T("Command list execution"));

	// execute command lists

	// Shadowmap
	_shadowMap->execute(context);

	// Depth pre pass
	auto commandList = _depthPrePassRenderer->commandList();
	if (commandList) {
		context->ExecuteCommandList(commandList, false);
	}

	// Main camera
	commandList = _mainCameraRenderer->commandList();
	if (commandList) {
		context->ExecuteCommandList(commandList, false);
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

