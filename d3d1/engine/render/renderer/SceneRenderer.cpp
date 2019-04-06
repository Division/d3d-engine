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
#include "PassRenderer.h"
#include "tbb/tbb.h"
#include "cvmarkersobj.h"

Concurrency::diagnostic::marker_series markers_render(_T("render"));

SceneRenderer::SceneRenderer() {
	_constantBufferManager = std::make_shared<ConstantBufferManager>();
	_inputLayoutCache = std::make_shared<InputLayoutCache>();
	_mainCameraRenderer = std::make_unique<PassRenderer>(RenderMode::Normal, _inputLayoutCache);
	_mainCameraRenderer->clearColor(true);
	_mainCameraRenderer->clearDepth(false);
	_depthPrePassRenderer = std::make_unique<PassRenderer>(RenderMode::DepthOnly, _inputLayoutCache);
	_depthPrePassRenderer->clearColor(false);
	_depthPrePassRenderer->clearDepth(true);
}

void SceneRenderer::renderScene(ScenePtr scene, ICameraParamsProviderPtr camera, ICameraParamsProviderPtr camera2D) {
	if (!camera) { return; }
	
	auto context = Engine::Get()->getD3DContext();
	{
		Concurrency::diagnostic::span s1(markers_render, _T("Multiple thread rendering"));

		scene->visibleObjects(camera);

		tbb::task_group g;
		g.run([&] {
			_mainCameraRenderer->render(scene, camera);
		});
		g.run([&] {
			_depthPrePassRenderer->render(scene, camera);
		});
		g.wait();
	}

	Concurrency::diagnostic::span s1(markers_render, _T("Command list execution"));

	auto commandList = _depthPrePassRenderer->commandList();
	if (commandList) {
		context->ExecuteCommandList(commandList, false);
	}

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

