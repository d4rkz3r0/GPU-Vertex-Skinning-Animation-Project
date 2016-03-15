#pragma once
#include "../../../../Shared/SharedUtils.h"

//First In Last Out Data Structure
class RenderTargetStack : public RTTI
{
	RTTI_DECLARATIONS(RenderTargetStack, RTTI)

public:
	RenderTargetStack();
	virtual ~RenderTargetStack();

	virtual void Begin() = 0;
	virtual void End() = 0;

protected:
	typedef struct RenderTargetData
	{
		RenderTargetData(UINT viewCount, ID3D11RenderTargetView** RenderTargetViews, ID3D11DepthStencilView* depthStencilView, const D3D11_VIEWPORT& viewport)	: ViewCount(viewCount), RenderTargetViews(RenderTargetViews), DepthStencilView(depthStencilView), Viewport(viewport) { }


		UINT ViewCount;
		ID3D11RenderTargetView** RenderTargetViews;
		ID3D11DepthStencilView* DepthStencilView;
		D3D11_VIEWPORT Viewport;

	} RenderTargetData;

	void Begin(ID3D11DeviceContext* deviceContext, UINT viewCount, ID3D11RenderTargetView** RenderTargetViews, ID3D11DepthStencilView* depthStencilView, const D3D11_VIEWPORT& viewport);
	void End(ID3D11DeviceContext* deviceContext);

private:
	RenderTargetStack(const RenderTargetStack& rhs) = delete;
	RenderTargetStack& operator=(const RenderTargetStack& rhs) = delete;

	static stack<RenderTargetData> mRenderTargetStack;
};