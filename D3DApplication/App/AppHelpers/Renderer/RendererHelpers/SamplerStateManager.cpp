#include "SamplerStateManager.h"
#include "../../../../Shared/SharedUtils.h"

SamplerStateManager::SamplerStateManager(ID3D11Device* pDevice) : mSamplerType(AnisoWrapSamplerState), mDevice(pDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);

	BuildSamplerStates();
}

SamplerStateManager::~SamplerStateManager()
{
	Destroy();
}


ID3D11SamplerState* SamplerStateManager::GetSamplerState(SamplerStateType SamplerState)
{
	return mSamplerStates[SamplerState];
}

void SamplerStateManager::BuildSamplerStates()
{
	D3D11_SAMPLER_DESC AnisoRepeatSamplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	AnisoRepeatSamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	AnisoRepeatSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	AnisoRepeatSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	AnisoRepeatSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	mDevice->CreateSamplerState(&AnisoRepeatSamplerDesc, &mSamplerStates[AnisoWrapSamplerState]);

	mDeviceContext->PSSetSamplers(0, 1, &mSamplerStates[AnisoWrapSamplerState]);

	mSamplerType = AnisoWrapSamplerState;
}

void SamplerStateManager::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
	for (auto sState : mSamplerStates)
	{
		ReleaseObject(sState);
	}
}

SamplerStateManager::SamplerStateType SamplerStateManager::GetType()
{
	return mSamplerType;
}