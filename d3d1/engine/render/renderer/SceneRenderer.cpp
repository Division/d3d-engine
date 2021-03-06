﻿#include "SceneRenderer.h"
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
#include "render/texture/RenderTarget.h"
#include "PassRenderer.h"
#include "system/Window.h"
#include "render/shading/LightGrid.h"
#include "render/posteffect/PostEffect.h"
#include "tbb/tbb.h"
#include "cvmarkersobj.h"

Concurrency::diagnostic::marker_series markers_render(_T("render"));

const unsigned int SHADOW_ATLAS_SIZE = 4096;

void SceneRenderer::projectorTexture(TexturePtr texture) {
	_projectorTexture = texture;
}

TexturePtr SceneRenderer::projectorTexture() const {
	return _projectorTexture;
}

SceneRenderer::SceneRenderer() {
	auto window = Engine::Get()->window();

	_constantBufferManager = std::make_shared<ConstantBufferManager>();
	_inputLayoutCache = std::make_shared<InputLayoutCache>();
	_shadowMap = std::make_unique<ShadowMap>(SHADOW_ATLAS_SIZE, SHADOW_ATLAS_SIZE, _inputLayoutCache);
	_lightGrid = std::make_shared<LightGrid>(32);

	auto renderTargetConfig = RenderTargetInitializer()
		.size(window->width(), window->height())
		.colorTarget(true, Engine::Get()->multisampleCount(), DXGI_FORMAT_R16G16B16A16_FLOAT)
		//.colorTarget(true, Engine::Get()->multisampleCount(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
		.depthTarget(false, DXGI_FORMAT_D24_UNORM_S8_UINT);

	_hdrRenderTarget = std::make_shared<RenderTarget>(renderTargetConfig);

	_postEffect = std::make_shared<PostEffect>(window->width(), window->height(), _inputLayoutCache);
	_mainCameraRenderer = std::make_unique<PassRenderer>(_hdrRenderTarget, _lightGrid, RenderMode::Normal, _inputLayoutCache);
	_mainCameraRenderer->clearColor(true);
	_mainCameraRenderer->clearDepth(false);
	_depthPrePassRenderer = std::make_unique<PassRenderer>(_hdrRenderTarget, nullptr, RenderMode::DepthOnly, _inputLayoutCache);
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

	_shadowMap->setupRenderPasses(_shadowCasters); // Should go BEFORE lightgrid setup

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

		auto projectorTexture = _projectorTexture;
		// Shadow map passes
		auto renderPasses = _shadowMap->renderPasses();
		for (int i = 0; i < _shadowMap->renderPassCount(); i++) {
			auto &pass = renderPasses[i];
			auto &shadowCaster = _shadowCasters[i];
			renderTaskGroup.run([&] {
				Concurrency::diagnostic::span s1(markers_render, _T("Shadow pass"));
				pass->render(scene, shadowCaster);
			});
		}

		// Depth pre pass
		renderTaskGroup.run([&] {
			Concurrency::diagnostic::span s1(markers_render, _T("depth pre pass"));
			_depthPrePassRenderer->render(scene, camera);
		});

		// Main camera
		renderTaskGroup.run([&] {
			Concurrency::diagnostic::span s1(markers_render, _T("main pass"));
			_mainCameraRenderer->setProjectorTexture(projectorTexture);
			_mainCameraRenderer->setShadowmapTexture(_shadowMap->depthAtlas());
			_mainCameraRenderer->render(scene, camera);
		});

		renderTaskGroup.wait(); // wait until all render passes completed
	}
	
	{
		// execute command lists
		Concurrency::diagnostic::span s1(markers_render, _T("Command list execution"));
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

	// Post processing
	_postEffect->render(context, _hdrRenderTarget->framebufferTexture(), Engine::Get()->renderTarget());
}

const uint32_t SceneRenderer::shadowAtlasSize() {
	return SHADOW_ATLAS_SIZE;
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

