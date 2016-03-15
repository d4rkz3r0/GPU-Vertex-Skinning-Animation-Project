#include "RasterizerStateManager.h"
#include "../../../../Shared/SharedUtils.h"

RasterizerStateManager::RasterizerStateManager(ID3D11Device* pDevice) : mRasterType(DefaultRasterState), mDevice(pDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);
	currRasterIndex = 0;
	prevRasterIndex = 0;

	BuildRasterizerStates();
}

RasterizerStateManager::~RasterizerStateManager()
{
	Destroy();
}

void RasterizerStateManager::SaveRasterizerState()
{
	prevRasterIndex = currRasterIndex;
	mRasterizerStates[currRasterIndex]->Release();
	mDeviceContext->RSGetState(&mRasterizerStates[currRasterIndex]);
}


ID3D11RasterizerState* RasterizerStateManager::GetRasterizerState(RasterStateType rasterizerState)
{
	return mRasterizerStates[rasterizerState];
}

void RasterizerStateManager::RestoreRasterizerState()
{
	mDeviceContext->RSSetState(mRasterizerStates[prevRasterIndex]);
}

void RasterizerStateManager::BuildRasterizerStates()
{
	D3D11_RASTERIZER_DESC desc;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;
	mDevice->CreateRasterizerState(&desc, &mRasterizerStates[DefaultRasterState]);
	desc.FillMode = D3D11_FILL_WIREFRAME;
	mDevice->CreateRasterizerState(&desc, &mRasterizerStates[WireFrameRasterState]);
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = TRUE;
	mDevice->CreateRasterizerState(&desc, &mRasterizerStates[CCW_RasterState]);
	desc.CullMode = D3D11_CULL_NONE;
	mDevice->CreateRasterizerState(&desc, &mRasterizerStates[NOCULL_RasterState]);
}

void RasterizerStateManager::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
	for (auto rState : mRasterizerStates)
	{
		ReleaseObject(rState);
	}
}

RasterizerStateManager::RasterStateType RasterizerStateManager::GetType()
{
	return mRasterType;
}