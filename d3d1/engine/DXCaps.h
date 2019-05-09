#pragma once

#include <d3d11_1.h>
#include <stdint.h>

namespace DXCaps {

	extern const int32_t CONSTANT_BUFFER_CONSTANT_SIZE;
	extern const int32_t CONSTANT_BUFFER_MAX_SIZE;
	extern const int32_t CONSTANT_BUFFER_ALIGNMENT;
	const D3D11_FEATURE_DATA_D3D11_OPTIONS &d3d11Options();
	const D3D11_FEATURE_DATA_D3D11_OPTIONS1 &d3d11Options1();
	bool ConstantBufferOffsetting();

	void initialize(ID3D11Device *device);
}