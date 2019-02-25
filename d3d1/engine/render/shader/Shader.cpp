#include "Shader.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include "../core/ID3DContextProvider.h"
#include "../system/Logging.h"
#include <map>
#include "ShaderResource.h"
#include "render/mesh/VertexAttrib.h"

Shader::Shader(ID3DContextProvider *provider) : _provider(provider) {}


struct ShaderConfig {
	std::vector<ShaderResourceName> resources;
	std::vector<ConstantBufferName> constantBuffers;
	std::vector<VertexAttrib> vertexAttribs;
};

typedef ShaderResourceName SRN;
typedef ConstantBufferName CBN;
typedef VertexAttrib VA;

// Stores info for every shader cap about what vertex attribs, 
// constant buffers and shader resources(textures) this cap has 
const std::map<ShaderCaps, ShaderConfig> SHADER_CAP_CONFIG = {
	{ ShaderCaps::ObjectData, { {}, { CBN::ObjectParams, CBN::Camera }, { VA::Position } } },
	{ ShaderCaps::Color, { {}, {}, {} } },
	{ ShaderCaps::Lighting, {
		{ SRN::ShadowMap, SRN::LightGrid, SRN::LightIndices, SRN::ProjectorTexture},
		{ CBN::Light, CBN::Projector }, { VA::Normal }
	} },
	{ ShaderCaps::NormalMap, { { SRN::NormalMap }, {}, { VA::Bitangent, VA::Tangent } } },
	{ ShaderCaps::Skinning, { {}, { CBN::SkinningMatrices }, { VA::JointWeights, VA::JointIndices } } },
	{ ShaderCaps::Texture0, { { SRN::Texture0 }, {}, { VA::TexCoord0 } } },
	{ ShaderCaps::Texture1, { { SRN::Texture1 }, {}, { VA::TexCoord1 } } },
	{ ShaderCaps::VertexColor, { {}, {}, { VA::VertexColor } } },
	{ ShaderCaps::SpecularMap, { { SRN::SpecularMap }, {}, {} } },
};

const std::map<VertexAttrib, std::string> SHADER_ATTRIB_DEFINES = {
  { VertexAttrib::Position, "aPosition" },
  { VertexAttrib::Normal, "aNormal" },
  { VertexAttrib::Tangent, "aTangent" },
  { VertexAttrib::Bitangent, "aBitangent" },
  { VertexAttrib::TexCoord0, "aTexCoord0" },
  { VertexAttrib::Corner, "aCorner" },
  { VertexAttrib::VertexColor, "aVertexColor" },
  { VertexAttrib::JointWeights, "aJointWeights" },
  { VertexAttrib::JointIndices, "aJointIndices" }
};

ID3D11InputLayout *Shader::createInputLayout(D3D11_INPUT_ELEMENT_DESC *inputDescription, int32_t count) {
	//if (!_ready) { return nullptr; }
	ID3D11InputLayout *result;
	auto device = _provider->getD3DDevice();
	device->CreateInputLayout(inputDescription, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), &result);
	return result;
}

void Shader::_initCaps(const ShaderCapsSet caps) {
	_caps = caps;
	_attribSet = VertexAttribSet();
	for (auto cap : _caps.caps()) {
		auto &config = SHADER_CAP_CONFIG.at((ShaderCaps)cap);
		for (auto attrib : config.vertexAttribs) {
			_attribSet.addCap(attrib);
		}
	}
}

std::vector<D3D_SHADER_MACRO> Shader::_getDefinesForCaps(const ShaderCapsSet & caps) {
	std::vector<D3D_SHADER_MACRO> result;

	for (auto attrib : _attribSet.caps()) {
		result.push_back({ SHADER_ATTRIB_DEFINES.at((VertexAttrib)attrib).c_str(), "1" });
	}

	return result;
}

void Shader::loadFromFile(const std::string &filename) {

}

void Shader::loadFromString(const std::string &shaderSource, const std::string &filename, ShaderCapsSet caps) {
	_initCaps(caps);

	auto context = _provider->getD3DContext();
	auto device = _provider->getD3DDevice();

	//std::wstring stemp = std::wstring(shaderSource.begin(), shaderSource.end());
	
	ID3DBlob *errors1, *errors2;

	auto defines = _getDefinesForCaps(caps);
	auto result1 = D3DCompile(shaderSource.c_str(), shaderSource.length(), NULL, &defines[0], NULL, "VShader", "vs_4_0", 0, 0, &_vsBlob, &errors1);
	auto result2 = D3DCompile(shaderSource.c_str(), shaderSource.length(), NULL, &defines[0], NULL, "PShader", "ps_4_0", 0, 0, &_psBlob, &errors2);

	_error = false;
	if (FAILED(result1)) {
		_error = true;
		ENGLog("[ERROR] Vertex shader compile error (\"%s\"):\n%s", filename.c_str(), (char*)errors1->GetBufferPointer());
		throw std::runtime_error("Error loading shader");
	}
	if (FAILED(result2)) {
		_error = true;
		ENGLog("[ERROR] Pixel shader compile error (\"%s\"):\n%s", filename.c_str(), (char*)errors2->GetBufferPointer());
		throw std::runtime_error("Error loading shader");
	}

	// encapsulate both shaders into shader objects
	if (!_error) {
		HRESULT result;
		result = device->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), NULL, &_vs);
		if (FAILED(result)) {
			_error = true;
			ENGLog("[ERROR] Can't create vertex shader \"%s\"", filename.c_str());
			throw std::runtime_error("Error loading shader");
		}

		result = device->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), NULL, &_ps);
		if (FAILED(result)) {
			_error = true;
			ENGLog("[ERROR] Can't create pixel shader \"%s\"", filename.c_str());
			throw std::runtime_error("Error loading shader");
		}
	}

	_ready = !_error;
	bind();

	if (_ready) {
		ENGLog("Shader loaded \"%s\"", filename.c_str());
	}
}

void Shader::bind()
{
	if (!_ready) {
		return;
	}

	auto context = _provider->getD3DContext();
	
	// set the shader objects
	context->VSSetShader(_vs, 0, 0);
	context->PSSetShader(_ps, 0, 0);
}


Shader::~Shader()
{
	if (_ready) {
		_vs->Release();
		_ps->Release();
		_vsBlob->Release();
		_psBlob->Release();
	}
}
