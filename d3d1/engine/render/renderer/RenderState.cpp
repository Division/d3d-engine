#include "RenderState.h"
#include "Engine.h"

RenderState::RenderState(ID3D11DeviceContext1 *context) : _context(context) {
	_depthOnlyState = _createDepthStencilState(true, true, D3D11_COMPARISON_LESS);
	_depthNormalState = _createDepthStencilState(true, false, D3D11_COMPARISON_LESS_EQUAL);
}

void RenderState::setRenderMode(RenderMode mode)
{
	auto engine = Engine::Get();
	auto renderTarget = engine->renderTargetView();
	auto depthStencil = engine->depthStencilView();

	switch (mode) {
	case RenderMode::DepthOnly:
		_context->OMSetRenderTargets(0, NULL, depthStencil);
		_context->OMSetDepthStencilState(_depthOnlyState, 1);
		break;

	case RenderMode::Normal:
		_context->OMSetRenderTargets(1, &renderTarget, depthStencil);
		_context->OMSetDepthStencilState(_depthNormalState, 1);
		break;

	default:
		throw std::runtime_error("Unsupported render mode");
	}
}

// Stencil isn't yet used
ID3D11DepthStencilState *RenderState::_createDepthStencilState(bool depthTest, bool depthWrite, D3D11_COMPARISON_FUNC depthFunc) {
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	auto device = Engine::Get()->getD3DDevice();

	// Depth test parameters
	depthStencilDesc.DepthEnable = depthTest;
	depthStencilDesc.DepthWriteMask = depthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = depthFunc;

	// Stencil test parameters
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	ID3D11DepthStencilState *state;
	ThrowIfFailed(device->CreateDepthStencilState(&depthStencilDesc, &state));

	return state;
}
