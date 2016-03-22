#include "RenderTargetStack.h"
#include "../../../BaseApp.h"

RTTI_DEFINITIONS(RenderTargetStack)

stack<RenderTargetStack::RenderTargetData> RenderTargetStack::mRenderTargetStack;

RenderTargetStack::RenderTargetStack() { }
RenderTargetStack::~RenderTargetStack() { }

void RenderTargetStack::Begin(ID3D11DeviceContext* deviceContext, UINT viewCount, ID3D11RenderTargetView** renderTargetViews, ID3D11DepthStencilView* depthStencilView, const D3D11_VIEWPORT& viewport)
{
	mRenderTargetStack.push(RenderTargetData(viewCount, renderTargetViews, depthStencilView, viewport));
	deviceContext->OMSetRenderTargets(viewCount, renderTargetViews, depthStencilView);
	deviceContext->RSSetViewports(1, &viewport);
}

void RenderTargetStack::End(ID3D11DeviceContext* deviceContext)
{
	mRenderTargetStack.pop();

	RenderTargetData renderTargetData = mRenderTargetStack.top();
	deviceContext->OMSetRenderTargets(renderTargetData.ViewCount, renderTargetData.RenderTargetViews, renderTargetData.DepthStencilView);
	deviceContext->RSSetViewports(1, &renderTargetData.Viewport);
}