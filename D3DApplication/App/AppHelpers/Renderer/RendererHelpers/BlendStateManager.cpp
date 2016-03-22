#include "BlendStateManager.h"
#include "../../../../Shared/SharedUtils.h"

BlendStateManager::BlendStateManager(ID3D11Device* pDevice) : mBlendType(OpaqueBlendState), mDevice(pDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);
	currBlendIndex = 0;
	prevBlendIndex = 0;

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
	//mDeviceContext->OMGetBlendState(&mBlendStates[currBlendIndex], mBlendFactor, &mSampleMask);
}


ID3D11BlendState* BlendStateManager::GetBlendState(BlendStateType BlendState)
{
	return mBlendStates[BlendState];
}

FLOAT* BlendStateManager::GetOpaqueBlendFactor()
{
	return mOBlendFactor;
}

FLOAT* BlendStateManager::GetTransparentBlendFactor()
{
	return mTBlendFactor;
}


UINT BlendStateManager::GetSampleMask()
{
	return mSampleMask;
}

void BlendStateManager::RestoreBlendState()
{
	//mDeviceContext->OMSetBlendState(mBlendStates[prevBlendIndex], mBlendFactor, mSampleMask);
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

	D3D11_BLEND_DESC alphaBlendStateDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	alphaBlendStateDesc.AlphaToCoverageEnable = FALSE;
	alphaBlendStateDesc.IndependentBlendEnable = FALSE;
	alphaBlendStateDesc.RenderTarget[0].BlendEnable = TRUE;
	alphaBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	alphaBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	mDevice->CreateBlendState(&alphaBlendStateDesc, &mBlendStates[AlphaCoverageBlendState]);

	alphaBlendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	alphaBlendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	mDevice->CreateBlendState(&alphaBlendStateDesc, &mBlendStates[AdditiveBlendState]);

	D3D11_BLEND_DESC transpBlendDesc;
	ZeroMemory(&transpBlendDesc, sizeof(transpBlendDesc));
	transpBlendDesc.AlphaToCoverageEnable = false;
	transpBlendDesc.RenderTarget[0].BlendEnable = true;
	transpBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	transpBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_BLEND_FACTOR;
	transpBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transpBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transpBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transpBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transpBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	mDevice->CreateBlendState(&transpBlendDesc, &mBlendStates[TransparencyBlendState]);

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