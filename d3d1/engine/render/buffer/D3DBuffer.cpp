//
// Created by Sidorenko Nikita on 4/11/18.
//

#include "D3DBuffer.h"
#include <stdexcept>
#include "system/Logging.h"
#include "core/ID3DContextProvider.h"

const int ALIGN_BYTES = 4;

D3DBuffer::D3DBuffer(ID3DContextProvider *provider, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usage, int32_t fixedSize)
	: _provider(provider), _bindFlag(bindFlag), _usage(usage), _fixedSize(fixedSize) {
	_recreateBuffer();
}

D3DBuffer::~D3DBuffer() {
	if (_buffer) {
		_buffer->Release();
		_buffer = nullptr;
	}
}

void D3DBuffer::_recreateBuffer() {
	auto targetSize = _data.size();

	if (_fixedSize > 0) {
		targetSize = _fixedSize;
		if (_size > _fixedSize) {
			throw std::runtime_error("D3DBuffer fixedSize exceeded");
		}
	}
	else if (_size == 0) {
		return;
	}

	_bufferAllocatedSize = (unsigned)targetSize;
	
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = _usage;                // write access access by CPU and GPU
	bufferDesc.ByteWidth = _bufferAllocatedSize;           
	bufferDesc.BindFlags = _bindFlag;       // use as a vertex buffer
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

	if (_buffer) {
		_buffer->Release();
		_buffer = nullptr;
	}
	auto device = _provider->getD3DDevice();
	device->CreateBuffer(&bufferDesc, nullptr, &_buffer);
}

void D3DBuffer::upload() {
	if (!_dirty || !_size) {
		return;
	}

	if (_bufferAllocatedSize < _data.size()) {
		_recreateBuffer();
	}

	auto context = _provider->getD3DContext();

	D3D11_MAPPED_SUBRESOURCE ms;
	auto mapMode = _usage == D3D11_USAGE_DYNAMIC ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE;
	context->Map(_buffer, NULL, mapMode, NULL, &ms);
	memcpy(ms.pData, &_data[0], _size);
	context->Unmap(_buffer, NULL);

	_dirty = false;
	this->resize(0);
}
