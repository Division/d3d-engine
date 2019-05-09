#include "DXCaps.h"
#include "system/Logging.h"

const int32_t DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE = 16;
const int32_t DXCaps::CONSTANT_BUFFER_MAX_SIZE = DXCaps::CONSTANT_BUFFER_CONSTANT_SIZE * D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT;
const int32_t DXCaps::CONSTANT_BUFFER_ALIGNMENT = 256;

D3D11_FEATURE_DATA_D3D11_OPTIONS _d3d11Options = {};
const D3D11_FEATURE_DATA_D3D11_OPTIONS &DXCaps::d3d11Options() { return _d3d11Options; }

D3D11_FEATURE_DATA_D3D11_OPTIONS1 _d3d11Options1 = {};
const D3D11_FEATURE_DATA_D3D11_OPTIONS1 &DXCaps::d3d11Options1() { return _d3d11Options1; }

bool DXCaps::ConstantBufferOffsetting() { return _d3d11Options.ConstantBufferOffsetting; };

void DXCaps::initialize(ID3D11Device *device) {
	if (FAILED(
		device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS, &_d3d11Options, sizeof(_d3d11Options))
	)) {
		ENGLog("%s", "ERROR: Unable to check feature support for D3D11_FEATURE_D3D11_OPTIONS");
	}

	if (FAILED(
		device->CheckFeatureSupport(D3D11_FEATURE_D3D11_OPTIONS1, &_d3d11Options1, sizeof(_d3d11Options1))
	)) {
		ENGLog("%s", "ERROR: Unable to check feature support for D3D11_FEATURE_D3D11_OPTIONS1");
	}
}