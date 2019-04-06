#include <d3d11_1.h>
#include "IRenderer.h"

class RenderState {
public:
	RenderState(ID3D11DeviceContext1 *context);
	void setRenderMode(RenderMode mode);

private:
	ID3D11DepthStencilState *_createDepthStencilState(bool depthTest, bool depthWrite, D3D11_COMPARISON_FUNC depthFunc);

private:
	ID3D11DepthStencilState *_depthOnlyState;
	ID3D11DepthStencilState *_depthNormalState;
	ID3D11DeviceContext1 *_context;
};