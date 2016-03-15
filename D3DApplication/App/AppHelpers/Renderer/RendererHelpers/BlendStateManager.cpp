#include "BlendStateManager.h"
#include "../../../../Shared/SharedUtils.h"

BlendStateManager::BlendStateManager(ID3D11Device* pDevice) : mBlendType(OpaqueBlendState), mDevice(pDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);
	currBlendIndex = 0;
	prevBlendIndex = 0;
	mSampleMask = 0xFFFFFFFF;

	BuildBlendStates();
}

BlendStateManager::~BlendStateManager()
{
	Destroy();
}

void BlendStateManager::SaveBlendState()
{
	prevBlendIndex = currBlendIndex;
	mBlendStates[currBlendIndex]->Release();
	mDeviceContext->OMGetBlendState(&mBlendStates[currBlendIndex], mBlendFactor, &mSampleMask);
}


ID3D11BlendState* BlendStateManager::GetBlendState(BlendStateType BlendState)
{
	return mBlendStates[BlendState];
}

FLOAT* BlendStateManager::GetBlendFactor()
{
	return mBlendFactor;
}

UINT BlendStateManager::GetSampleMask()
{
	return mSampleMask;
}

void BlendStateManager::RestoreBlendState()
{
	mDeviceContext->OMSetBlendState(mBlendStates[prevBlendIndex], mBlendFactor, mSampleMask);
}

void BlendStateManager::BuildBlendStates()
{
	D3D11_BLEND_DESC opaqueBlendStateDesc;
	ZeroMem(opaqueBlendStateDesc);
	opaqueBlendStateDesc.AlphaToCoverageEnable = FALSE;
	opaqueBlendStateDesc.IndependentBlendEnable = FALSE;
	opaqueBlendStateDesc.RenderTarget[0].BlendEnable = FALSE;
	opaqueBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	opaqueBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	opaqueBlendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	opaqueBlendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	opaqueBlendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	opaqueBlendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	opaqueBlendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	mDevice->CreateBlendState(&opaqueBlendStateDesc, &mBlendStates[OpaqueBlendState]);
	mBlendFactor = new FLOAT[4];
	mBlendFactor[0] = 1.0f;
	mBlendFactor[1] = 1.0f;
	mBlendFactor[2] = 1.0f;

	D3D11_BLEND_DESC alphaBlendStateDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	alphaBlendStateDesc.AlphaToCoverageEnable = FALSE;
	alphaBlendStateDesc.IndependentBlendEnable = FALSE;
	alphaBlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	alphaBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	mDevice->CreateBlendState(&alphaBlendStateDesc, &mBlendStates[AlphaBlendState]);
}

void BlendStateManager::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
	for (auto bState : mBlendStates)
	{
		ReleaseObject(bState);
	}
}

BlendStateManager::BlendStateType BlendStateManager::GetType()
{
	return mBlendType;
}