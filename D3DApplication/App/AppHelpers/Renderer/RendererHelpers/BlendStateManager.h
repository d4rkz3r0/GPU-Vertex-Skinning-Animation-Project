#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
typedef unsigned int UINT;

class BlendStateManager
{
	friend class PipelineState;
public:
	enum BlendStateType
	{
		OpaqueBlendState = 0,
		AlphaCoverageBlendState,
		AdditiveBlendState,
		TransparencyBlendState,
		Count_OfBlendStates
	};

	BlendStateManager(ID3D11Device* theDevice);
	virtual ~BlendStateManager();

	void SaveBlendState();
	ID3D11BlendState* GetBlendState(BlendStateType BlendState);
	FLOAT* GetOpaqueBlendFactor();
	FLOAT* GetTransparentBlendFactor();
	UINT GetSampleMask();
	void RestoreBlendState();
	void Destroy();
	BlendStateType GetType();

private:
	BlendStateType	mBlendType;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11BlendState* mBlendStates[Count_OfBlendStates];
	float mOBlendFactor[4] = { NULL, NULL, NULL, NULL };
	float mTBlendFactor[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
	UINT mSampleMask = 0xFFFFFFFF;
	UINT currBlendIndex;;
	UINT prevBlendIndex;
	void BuildBlendStates();
};