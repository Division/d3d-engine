#pragma once

#include <string>
#include "core/IEngineResource.h"
#include "render/mesh/VertexAttrib.h"
#include "ShaderCaps.h"
#include <vector>

class ID3DContextProvider;

// TODO: check loading here https://github.com/mhyhre/Tuxis-Engine/blob/master/Sources/Shader.cpp#L16
// https://docs.microsoft.com/en-us/windows/desktop/api/d3dcompiler/nf-d3dcompiler-d3dcompile

class Shader : public IEngineResource {
public:
	Shader(ID3DContextProvider *provider);
	~Shader();

	void loadFromFile(const std::string &filename, ShaderCapsSet caps = ShaderCapsSet());
	void loadFromString(const char *shaderSource, size_t length, const std::string &filename, ShaderCapsSet caps);
	void bind();
	ID3D11InputLayout *createInputLayout(D3D11_INPUT_ELEMENT_DESC *inputDescription, int32_t count);

	const ShaderCapsSet &capsSet() const { return _caps; }
	const VertexAttribSet &vertexAttribSet() const { return _attribSet; }

	// IEngineResource
	bool isLoading() const override { return false; };
	bool isReady() const override { return _ready; };

private:
	void _initCaps(const ShaderCapsSet caps);
	std::vector<D3D_SHADER_MACRO> _getDefinesForCaps(const ShaderCapsSet &caps);

private:
	VertexAttribSet _attribSet;
	ShaderCapsSet _caps;

	bool _error = false;
	bool _ready = false;

	ID3DContextProvider *_provider;
	ID3D10Blob *_vsBlob;
	ID3D10Blob *_psBlob;
	ID3D11VertexShader *_vs;               // the pointer to the vertex shader
	ID3D11PixelShader *_ps;                // the pointer to the pixel shader

	std::vector<D3D11_INPUT_ELEMENT_DESC> _inputLayout;
};

typedef std::shared_ptr<Shader> ShaderPtr;
