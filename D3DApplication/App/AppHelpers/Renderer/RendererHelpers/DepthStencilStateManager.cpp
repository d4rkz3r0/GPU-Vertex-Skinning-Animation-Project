#include "DepthStencilStateManager.h"
#include "../../../../Shared/SharedUtils.h"

DepthStencilStateManager::DepthStencilStateManager(ID3D11Device* pDevice) : mDepthStencilType(DepthStencilStateLessEqual), mDevice(pDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);
	currDepthStencilIndex = 0;
	prevDepthStencilIndex = 0;

	BuildDepthStencilStates();
}

DepthStencilStateManager::~DepthStencilStateManager()
{
	Destroy();
}

void DepthStencilStateManager::SaveDepthStencilState()
{
	prevDepthStencilIndex = currDepthStencilIndex;
	mDepthStencilStates[currDepthStencilIndex]->Release();
	mDeviceContext->OMGetDepthStencilState(&mDepthStencilStates[currDepthStencilIndex], &mStencilRefs[currDepthStencilIndex]);
}


ID3D11DepthStencilState* DepthStencilStateManager::GetDepthStencilState(DepthStencilStateType DepthStencilState)
{
	return mDepthStencilStates[DepthStencilState];
}

UINT DepthStencilStateManager::GetDepthStencilStateRef(DepthStencilStateType depthStencilState)
{
	switch (depthStencilState)
	{
	case DepthStencilStateLessEqual:
	case DepthStencilStateNoDepth:
		return 0;
	default:
		return 0;
	}
}

void DepthStencilStateManager::RestoreDepthStencilState()
{
	mDeviceContext->OMSetDepthStencilState(mDepthStencilStates[prevDepthStencilIndex], mStencilRefs[prevDepthStencilIndex]);
}

void DepthStencilStateManager::BuildDepthStencilStates()
{
	D3D11_DEPTH_STENCIL_DESC dssDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	dssDesc.DepthEnable = TRUE;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	mDevice->CreateDepthStencilState(&dssDesc, &mDepthStencilStates[DepthStencilStateLessEqual]);
	mStencilRefs[DepthStencilStateLessEqual] = 0;

	dssDesc.DepthEnable = FALSE;
	mDevice->CreateDepthStencilState(&dssDesc, &mDepthStencilStates[DepthStencilStateNoDepth]);
	mStencilRefs[DepthStencilStateNoDepth] = 0;
}

void DepthStencilStateManager::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
	for (auto dState : mDepthStencilStates)
	{
		ReleaseObject(dState);
	}
}

DepthStencilStateManager::DepthStencilStateType DepthStencilStateManager::GetType()
{
	return mDepthStencilType;
}