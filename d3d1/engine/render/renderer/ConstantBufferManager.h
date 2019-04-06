#pragma once

#include <memory>
#include "RenderOperation.h"
#include "core/ID3DContextProvider.h"
#include "Engine.h"

class D3DMemoryBuffer;

// Created for every render pass (thread)
class PassConstantBufferManager : public ID3DContextProvider {
public:
	PassConstantBufferManager(ID3D11DeviceContext1 *context);

	void upload();
	void setObjectParamsBlock(RenderOperation *rop);
	void setObjectParams(const MultiBufferAddress &address);
	void setSkinningMatricesBlock(RenderOperation *rop);
	void setSkinningMatrices(const MultiBufferAddress &address);
	uint32_t addCamera(std::shared_ptr<ICameraParamsProvider> camera);
	void activateCamera(uint32_t offset);

	ID3D11DeviceContext1 *getD3DContext() { return _context;  };
	ID3D11Device1 *getD3DDevice() { return Engine::Get()->getD3DDevice(); };

private:
	ID3D11DeviceContext1 *_context;
	std::unique_ptr<D3DMemoryBuffer> _objectParams;
	std::unique_ptr<D3DMemoryBuffer> _skinningMatrices;
	std::unique_ptr<D3DMemoryBuffer> _cameraData;
};

class ConstantBufferManager {
public:
	ConstantBufferManager();

	void upload();
	uint32_t addCamera(std::shared_ptr<ICameraParamsProvider> camera);
	void activateCamera(uint32_t offset);

private:
	std::unique_ptr<D3DMemoryBuffer> _cameraData;
};