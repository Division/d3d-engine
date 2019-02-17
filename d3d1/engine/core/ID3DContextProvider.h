#pragma once

#include <d3d11.h>

class ID3DContextProvider {
public:
	virtual ID3D11DeviceContext *getD3DContext() = 0;
	virtual ID3D11Device *getD3DDevice() = 0;
	virtual ~ID3DContextProvider() = default;
};
