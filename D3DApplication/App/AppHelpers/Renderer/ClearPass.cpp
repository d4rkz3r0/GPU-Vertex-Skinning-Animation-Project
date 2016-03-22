#include "ClearPass.h"
#include <d3d11.h>
#include "../../../Shared/SharedUtils.h"


ClearPass::ClearPass(ID3D11Device* pDevice, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV) : mDevice(pDevice), mRTV(pRTV), mDSV(pDSV), m_Enabled(true)
{
	mDevice->GetImmediateContext(&mDeviceContext);
}

ClearPass::~ClearPass()
{
	Destroy();
}

void ClearPass::SetEnabled(bool enabled)
{
	m_Enabled = enabled;
}

bool ClearPass::IsEnabled() const
{
	return m_Enabled;
}

void ClearPass::Render()
{
	mDeviceContext->ClearRenderTargetView(mRTV, reinterpret_cast<const float*>(&mClearColor));
	mDeviceContext->ClearDepthStencilView(mDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


void ClearPass::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
	ReleaseObject(mRTV);
	ReleaseObject(mDSV);
}