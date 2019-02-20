#pragma once

#include <d3d11_1.h>

class ID3DContextProvider {
public:
	virtual ID3D11DeviceContext1 *getD3DContext() = 0;
	virtual ID3D11Device1 *getD3DDevice() = 0;
	virtual ~ID3DContextProvider() = default;
};
