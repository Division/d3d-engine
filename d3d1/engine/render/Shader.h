#pragma once

#include <string>
#include "core/IEngineResource.h"

class ID3DContextProvider;

// TODO: check loading here https://github.com/mhyhre/Tuxis-Engine/blob/master/Sources/Shader.cpp#L16
// https://docs.microsoft.com/en-us/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompile

class Shader : public IEngineResource {
public:
	Shader(ID3DContextProvider *provider);
	~Shader();

	void loadFromFile(const std::string &filename);
	void bind();
	ID3D11InputLayout *createInputLayout(D3D11_INPUT_ELEMENT_DESC *inputDescription, int32_t count);

	// IEngineResource
	bool isLoading() const override { return false; };
	bool isReady() const override { return _ready; };

private:
	bool _error = false;
	bool _ready = false;
	ID3DContextProvider *_provider;
	ID3D10Blob *_vsBlob;
	ID3D10Blob *_psBlob;
	ID3D11VertexShader *_vs;               // the pointer to the vertex shader
	ID3D11PixelShader *_ps;                // the pointer to the pixel shader
};

