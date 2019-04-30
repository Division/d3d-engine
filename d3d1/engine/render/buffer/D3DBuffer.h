#pragma once

#include <vector>
#include <memory>
#include "MemoryBuffer.h"
#include <d3d11_1.h>

class ID3DContextProvider;

class D3DBuffer {
public:
	D3DBuffer(ID3DContextProvider *provider, D3D11_BUFFER_DESC bufferDesc, uint32_t size, void *data = nullptr);
	D3DBuffer(ID3DContextProvider *provider, uint32_t bindFlag, D3D11_USAGE usage, uint32_t size, void *data = nullptr, uint32_t miscFlags = 0, uint32_t stride = 0);

	~D3DBuffer();

	uint32_t size() const { return _size; }
	void *map();
	void unmap();

	ID3D11Buffer *buffer() { return _buffer;  }


protected:
	ID3DContextProvider *_provider;
	ID3D11Buffer* _buffer = nullptr;
	uint32_t _bindFlag; 
	D3D11_USAGE _usage;
	uint32_t _miscFlags = 0;
	uint32_t _size;
	uint32_t _stride;
};
