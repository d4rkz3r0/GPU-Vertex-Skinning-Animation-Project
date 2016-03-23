#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

typedef unsigned int UINT;

class SamplerStateManager
{
public:
	enum SamplerStateType
	{
		AnisoWrapSamplerState = 0,
		Count_OfSamplerStates
	};

	SamplerStateManager(ID3D11Device* theDevice);
	virtual ~SamplerStateManager();

	ID3D11SamplerState* GetSamplerState(SamplerStateType SamplerState);
	void Destroy();
	SamplerStateType GetType();

private:
	SamplerStateType mSamplerType;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11SamplerState* mSamplerStates[Count_OfSamplerStates];

	void BuildSamplerStates();
};