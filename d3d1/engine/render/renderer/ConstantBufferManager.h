#pragma once

#include <memory>

class D3DMemoryBuffer;
class RenderOperation;

class ConstantBufferManager {
public:
	ConstantBufferManager();

	void setObjectParamsBlock(RenderOperation *rop);
	void upload();

private:
	std::unique_ptr<D3DMemoryBuffer> _objectParams;
	std::unique_ptr<D3DMemoryBuffer> _cameraData;
};