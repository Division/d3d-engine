﻿#pragma once

#include <d3d11_1.h>

#pragma once

#include <vector>
#include "MemoryBuffer.h"
#include <d3d11_1.h>

class ID3DContextProvider;
class D3DBuffer;

class D3DMemoryBuffer : public MemoryBuffer {
public:
	D3DMemoryBuffer(ID3DContextProvider *provider, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usage, uint32_t fixedSize = 0);

	void upload();
	uint32_t size() const;

	ID3D11Buffer *buffer();

protected:
	void _recreateBuffer();

protected:
	ID3DContextProvider *_provider;
	std::shared_ptr<D3DBuffer> _buffer;
	D3D11_BIND_FLAG _bindFlag;
	D3D11_USAGE _usage;
	uint32_t _fixedSize;

	uint32_t _bufferAllocatedSize = 0;
};