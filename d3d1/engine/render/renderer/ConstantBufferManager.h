#pragma once

#include <memory>
#include "RenderOperation.h"

class D3DMemoryBuffer;

class ConstantBufferManager {
public:
	ConstantBufferManager();

	void setObjectParamsBlock(RenderOperation *rop);
	void upload();
	void setObjectParams(const MultiBufferAddress &address);
	uint32_t addCamera(std::shared_ptr<ICameraParamsProvider> camera);
	void activateCamera(uint32_t offset);

private:
	std::unique_ptr<D3DMemoryBuffer> _objectParams;
	std::unique_ptr<D3DMemoryBuffer> _cameraData;
};