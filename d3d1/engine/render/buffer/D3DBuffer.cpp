//
// Created by Sidorenko Nikita on 4/11/18.
//

#include "D3DBuffer.h"
#include <stdexcept>
#include "system/Logging.h"
#include "core/ID3DContextProvider.h"

const int ALIGN_BYTES = 4;

D3DBuffer::D3DBuffer(ID3DContextProvider *provider, D3D11_BIND_FLAG bindFlag, D3D11_USAGE usage, int32_t size, void *data)
	: _provider(provider), _bindFlag(bindFlag), _usage(usage), _size(size) {

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	bufferDesc.Usage = _usage;
	bufferDesc.ByteWidth = _size;
	bufferDesc.BindFlags = _bindFlag;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	auto device = _provider->getD3DDevice();
	auto result = device->CreateBuffer(&bufferDesc, data ? &initData : nullptr, &_buffer);
	if (FAILED(result)) {
		ENGLog("[ERROR] Can't create buffer");
	}
}

D3DBuffer::~D3DBuffer() {
	if (_buffer) {
		_buffer->Release();
		_buffer = nullptr;
	}
}


void *D3DBuffer::map() {
	auto context = _provider->getD3DContext();
	D3D11_MAPPED_SUBRESOURCE ms;
	auto mapMode = _usage == D3D11_USAGE_DYNAMIC ? D3D11_MAP_WRITE_DISCARD : D3D11_MAP_WRITE;
	context->Map(_buffer, NULL, mapMode, NULL, &ms);
	return ms.pData;
}

void D3DBuffer::unmap() {
	auto context = _provider->getD3DContext();
	context->Unmap(_buffer, NULL);
}
