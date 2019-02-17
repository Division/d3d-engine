#pragma once

#include <vector>
#include <memory>
#include "MemoryBuffer.h"
#include <d3d11.h>

class ID3DContextProvider;

class D3DBuffer : public MemoryBuffer {
public:
 // D3DBuffer(GLenum target, GLenum usage, unsigned int fixedSize = 0);
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

  /*
  virtual void bind();
  virtual void unbind();
  void upload() override;
  GLuint vbo() { return _vbo; }

protected:
  unsigned int _bufferAllocatedSize = 0;
  GLuint _vbo = 0;
  GLenum _target;
  GLenum _usage;
  unsigned int _fixedSize;
  void _recreateBuffer();

  */
};


//typedef std::shared_ptr<VertexBufferObject> VertexBufferObjectPtr;

/*
class SwappableVertexBufferObject : public SwappableBufferObject<VertexBufferObject> {
public:
  SwappableVertexBufferObject(GLenum target, GLenum usage, unsigned int fixedSize = 0, unsigned int count = 2)
      : SwappableBufferObject<VertexBufferObject>(count), _target(target), _usage(usage), _fixedSize(fixedSize) {
    _createBuffers();
  }

protected:
  std::shared_ptr<VertexBufferObject> createBuffer() override;

private:
  GLenum _target;
  GLenum _usage;
  unsigned int _fixedSize;
};

typedef std::shared_ptr<SwappableVertexBufferObject> SwappableVertexBufferObjectPtr;
*/
