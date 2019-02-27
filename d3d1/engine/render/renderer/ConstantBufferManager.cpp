#include "ConstantBufferManager.h"
#include "DXCaps.h"
#include <Engine.h>
#include "RenderOperation.h"
#include "render/buffer/D3DMemoryBuffer.h"

ConstantBufferManager::ConstantBufferManager() {
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

void ConstantBufferManager::setObjectParamsBlock(RenderOperation *rop) {
	if (rop->objectParams) {
		auto address = _objectParams->appendData((void *)rop->objectParams, sizeof(ConstantBufferStruct::ObjectParams));
		rop->objectParamsBlockOffset.offset = address;
		rop->objectParamsBlockOffset.index = 0;
	}
}

void ConstantBufferManager::upload() {
	_objectParams->upload();
}

