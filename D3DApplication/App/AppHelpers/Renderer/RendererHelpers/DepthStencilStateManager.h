#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

typedef unsigned int UINT;

class DepthStencilStateManager
{
	friend class PipelineState;

public:
	enum DepthStencilStateType
	{
		DepthStencilStateLessEqual,
		DepthStencilStateNoDepth,
		Count_OfDepthStencilStates
	};

	DepthStencilStateManager(ID3D11Device* theDevice);
	virtual ~DepthStencilStateManager();

	void SaveDepthStencilState();
	ID3D11DepthStencilState* GetDepthStencilState(DepthStencilStateType DepthStencilState);
	UINT GetDepthStencilStateRef(DepthStencilStateType depthStencilState);
	void RestoreDepthStencilState();
	void Destroy();
	DepthStencilStateType GetType();

private:
	DepthStencilStateType mDepthStencilType;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11DepthStencilState* mDepthStencilStates[Count_OfDepthStencilStates];
	UINT mStencilRefs[Count_OfDepthStencilStates];

	UINT currDepthStencilIndex;
	UINT prevDepthStencilIndex;

	void BuildDepthStencilStates();
};