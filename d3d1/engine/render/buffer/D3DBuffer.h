#pragma once

#include <vector>
#include <memory>
#include "MemoryBuffer.h"
#include <d3d11_1.h>

class ID3DContextProvider;

class D3DBuffer {
public:
	D3DBuffer(ID3DContextProvider *provider, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usage, uint32_t size, void *data = nullptr);
	~D3DBuffer();

	uint32_t size() const { return _size; }
	void *map();
	void unmap();

	ID3D11Buffer *buffer() { return _buffer;  }


protected:
	ID3DContextProvider *_provider;
	ID3D11Buffer* _buffer = nullptr;
	D3D11_BIND_FLAG _bindFlag; 
	D3D11_USAGE _usage;
	uint32_t _size;
};
