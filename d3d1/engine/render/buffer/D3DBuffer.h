#pragma once

#include <vector>
#include <memory>
#include "MemoryBuffer.h"
#include <d3d11.h>

class ID3DContextProvider;

class D3DBuffer : public MemoryBuffer {
public:
	D3DBuffer(ID3DContextProvider *provider, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usage, int32_t fixedSize = 0);
	~D3DBuffer() override;

	void upload();

	ID3D11Buffer *buffer() { return _buffer;  }

protected:
	void _recreateBuffer();

protected:
	ID3DContextProvider *_provider;
	ID3D11Buffer* _buffer = nullptr;
	D3D11_BIND_FLAG _bindFlag; 
	D3D11_USAGE _usage;
	int32_t _fixedSize;

	unsigned int _bufferAllocatedSize = 0;
};
