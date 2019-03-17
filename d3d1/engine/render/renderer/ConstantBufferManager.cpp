#include "ConstantBufferManager.h"
#include "DXCaps.h"
#include <Engine.h>
#include "RenderOperation.h"
#include "render/buffer/D3DMemoryBuffer.h"
#include "render/shader/ShaderResource.h"
#include "ICameraParamsProvider.h"

ConstantBufferManager::ConstantBufferManager() {
	_objectParams = std::make_unique<D3DMemoryBuffer>(Engine::Get(),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE_DYNAMIC
	);
	_cameraData = std::make_unique<D3DMemoryBuffer>(Engine::Get(),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE_DYNAMIC
	);
	_skinningMatrices = std::make_unique<D3DMemoryBuffer>(Engine::Get(),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE_DYNAMIC
	);
}

void ConstantBufferManager::upload() {
	_objectParams->upload();
	_cameraData->upload();
	_skinningMatrices->upload();
}

void ConstantBufferManager::setObjectParamsBlock(RenderOperation *rop) {
	if (rop->objectParams) {
		auto address = _objectParams->appendData((void *)rop->objectParams, sizeof(ConstantBufferStruct::ObjectParams), DXCaps::CONSTANT_BUFFER_ALIGNMENT);
		rop->objectParamsBlockOffset.offset = address;
		rop->objectParamsBlockOffset.index = 0;
	}
}

void ConstantBufferManager::setSkinningMatricesBlock(RenderOperation *rop) {
	if (rop->skinningMatrices) {
		auto address = _skinningMatrices->appendData((void *)rop->skinningMatrices, sizeof(ConstantBufferStruct::SkinningMatrices), DXCaps::CONSTANT_BUFFER_ALIGNMENT);
		rop->skinningOffset.offset = address;
		rop->skinningOffset.index = 0;
	}
}

void ConstantBufferManager::setObjectParams(const MultiBufferAddress &address) {
	auto constantBuffer = _objectParams->buffer();
	UINT firstConstant = address.offset / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE;
	UINT constantCount = DXCaps::CONSTANT_BUFFER_ALIGNMENT / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE;

	auto context = Engine::Get()->getD3DContext();
	context->VSSetConstantBuffers1((UINT)ConstantBufferName::ObjectParams, 1, &constantBuffer, &firstConstant, &constantCount);
	context->PSSetConstantBuffers1((UINT)ConstantBufferName::ObjectParams, 1, &constantBuffer, &firstConstant, &constantCount);
}

void ConstantBufferManager::setSkinningMatrices(const MultiBufferAddress &address) {
	auto constantBuffer = _skinningMatrices->buffer();
	UINT firstConstant = address.offset / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE;
	UINT constantCount = sizeof(ConstantBufferStruct::SkinningMatrices) / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE;
	constantCount = (UINT)ceilf((float)constantCount / 16.0f) * 16;

	auto context = Engine::Get()->getD3DContext();
	context->VSSetConstantBuffers1((UINT)ConstantBufferName::SkinningMatrices, 1, &constantBuffer, &firstConstant, &constantCount);
	context->PSSetConstantBuffers1((UINT)ConstantBufferName::SkinningMatrices, 1, &constantBuffer, &firstConstant, &constantCount);
}

uint32_t ConstantBufferManager::addCamera(std::shared_ptr<ICameraParamsProvider> camera) {
	ConstantBufferStruct::Camera cameraData;
	cameraData.position = camera->cameraPosition();
	cameraData.screenSize = camera->cameraViewSize();
	cameraData.viewMatrix = camera->cameraViewMatrix();
	cameraData.projectionMatrix = camera->cameraProjectionMatrix();

	auto address = _cameraData->appendData(&cameraData, sizeof(ConstantBufferStruct::Camera), DXCaps::CONSTANT_BUFFER_ALIGNMENT);
	camera->cameraIndex(address);
	return address;
}

void ConstantBufferManager::activateCamera(uint32_t offset) {
	auto constantBuffer = _cameraData->buffer();
	UINT firstConstant = offset / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE;
	UINT constantCount = DXCaps::CONSTANT_BUFFER_ALIGNMENT / DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE;

	auto context = Engine::Get()->getD3DContext();
	context->VSSetConstantBuffers1((UINT)ConstantBufferName::Camera, 1, &constantBuffer, &firstConstant, &constantCount);
	context->PSSetConstantBuffers1((UINT)ConstantBufferName::Camera, 1, &constantBuffer, &firstConstant, &constantCount);
}