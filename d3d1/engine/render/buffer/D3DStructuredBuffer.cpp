
#include "D3DStructuredBuffer.h"
#include <stdexcept>
#include "system/Logging.h"
#include "core/ID3DContextProvider.h"
#include "D3DBuffer.h"
#include "utils/Math.h"
#include "Engine.h"

const int ALIGN_BYTES = 4;

D3DStructuredBuffer::D3DStructuredBuffer(ID3DContextProvider *provider, uint32_t bindFlag, D3D11_USAGE usage, uint32_t stride)
	: _provider(provider), _bindFlag(bindFlag), _usage(usage), _stride(stride) {
	_miscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	_recreateBuffer();
}

D3DStructuredBuffer::~D3DStructuredBuffer()
{
	SafeRelease(&_shaderResourceView);
}

void D3DStructuredBuffer::_recreateBuffer() {
	auto targetSize = _data.size();

	if (_fixedSize > 0) {
		targetSize = _fixedSize;
		if ((uint32_t)_size > _fixedSize) {
			throw std::runtime_error("D3DBuffer fixedSize exceeded");
		}
	}
	else if (_size == 0) {
		return;
	}

	_bufferAllocatedSize = targetSize;//(uint32_t)getPowerOfTwo((int32_t)targetSize);
	_buffer = std::make_shared<D3DBuffer>(_provider, _bindFlag, _usage, _bufferAllocatedSize, nullptr, _miscFlags, _stride);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.ElementWidth = _bufferAllocatedSize / _stride;
	auto device = _provider->getD3DDevice();
	SafeRelease(&_shaderResourceView);
	ThrowIfFailed(device->CreateShaderResourceView(_buffer->buffer(), &srvDesc, &_shaderResourceView));
}


void D3DStructuredBuffer::upload() {
	if (!_dirty || !_size) {
		return;
	}

	if (_bufferAllocatedSize < _data.size()) {
		_recreateBuffer();
	}

	auto bufferPointer = _buffer->map();
	memcpy(bufferPointer, &_data[0], _size);
	_buffer->unmap();

	_dirty = false;
	this->resize(0);
}

uint32_t D3DStructuredBuffer::size() const
{
	return _buffer->size();
}

ID3D11Buffer *D3DStructuredBuffer::buffer()
{
	return _buffer->buffer();
}
