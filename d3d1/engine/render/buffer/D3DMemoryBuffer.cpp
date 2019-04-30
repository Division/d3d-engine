#include "D3DMemoryBuffer.h"
#include <stdexcept>
#include "system/Logging.h"
#include "core/ID3DContextProvider.h"
#include "D3DBuffer.h"
#include "utils/Math.h"

const int ALIGN_BYTES = 4;

D3DMemoryBuffer::D3DMemoryBuffer(ID3DContextProvider *provider, uint32_t bindFlag, D3D11_USAGE usage, uint32_t fixedSize, uint32_t miscFlags, uint32_t stride)
	: _provider(provider), _bindFlag(bindFlag), _usage(usage), _fixedSize(fixedSize), _miscFlags(miscFlags), _stride(stride) {
	_recreateBuffer();
}

void D3DMemoryBuffer::_recreateBuffer() {
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

	_bufferAllocatedSize = (uint32_t)getPowerOfTwo((int32_t)targetSize);
	_buffer = std::make_shared<D3DBuffer>(_provider, _bindFlag, _usage, _bufferAllocatedSize, nullptr, _miscFlags, _stride);
}


void D3DMemoryBuffer::upload() {
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

uint32_t D3DMemoryBuffer::size() const
{
	return _buffer->size();
}

ID3D11Buffer *D3DMemoryBuffer::buffer()
{
	return _buffer->buffer();
}
