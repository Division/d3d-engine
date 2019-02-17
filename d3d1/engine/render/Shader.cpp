#include "Shader.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../core/ID3DContextProvider.h"
#include "../system/Logging.h"

Shader::Shader(ID3DContextProvider *provider) : _provider(provider) {}

ID3D11InputLayout *Shader::createInputLayout(D3D11_INPUT_ELEMENT_DESC *inputDescription, int32_t count) {
	//if (!_ready) { return nullptr; }
	ID3D11InputLayout *result;
	auto device = _provider->getD3DDevice();
	device->CreateInputLayout(inputDescription, count, _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), &result);
	return result;
}

void Shader::loadFromFile(const std::string &filename) {
	auto context = _provider->getD3DContext();
	auto device = _provider->getD3DDevice();

	// load and compile the two shaders

	std::wstring stemp = std::wstring(filename.begin(), filename.end());
	
	ID3DBlob *errors1, *errors2;

	auto result1 = D3DCompileFromFile(stemp.c_str(), 0, 0, "VShader", "vs_4_0", 0, 0, &_vsBlob, &errors1);
	auto result2 = D3DCompileFromFile(stemp.c_str(), 0, 0, "PShader", "ps_4_0", 0, 0, &_psBlob, &errors2);

	_error = false;
	if (FAILED(result1)) {
		_error = true;
		ENGLog("[ERROR] Vertex shader compile error (\"%s\"):\n%s", filename.c_str(), (char*)errors1->GetBufferPointer());
	}
	if (FAILED(result2)) {
		_error = true;
		ENGLog("[ERROR] Pixel shader compile error (\"%s\"):\n%s", filename.c_str(), (char*)errors2->GetBufferPointer());
	}

	// encapsulate both shaders into shader objects
	if (!_error) {
		HRESULT result;
		result = device->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), NULL, &_vs);
		if (FAILED(result)) {
			_error = true;
			ENGLog("[ERROR] Can't create vertex shader \"%s\"", filename.c_str());
		}

		result = device->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), NULL, &_ps);
		if (FAILED(result)) {
			_error = true;
			ENGLog("[ERROR] Can't create pixel shader \"%s\"", filename.c_str());
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
