#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

typedef unsigned int UINT;

class RasterizerStateManager
{
	friend class PipelineState;
public:
	enum RasterStateType
	{
		DefaultRasterState = 0,
		WireFrameRasterState,
		CCW_RasterState,
		NOCULL_RasterState,
		Count_OfRasterStates
	};

	RasterizerStateManager(ID3D11Device* theDevice);
	virtual ~RasterizerStateManager();

	void SaveRasterizerState();
	ID3D11RasterizerState* GetRasterizerState(RasterStateType rasterizerState);
	void RestoreRasterizerState();
	void Destroy();
	RasterStateType GetType();

private:
	RasterStateType	mRasterType;

	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11RasterizerState* mRasterizerStates[Count_OfRasterStates];
	
	UINT currRasterIndex;;
	UINT prevRasterIndex;

	void BuildRasterizerStates();

};