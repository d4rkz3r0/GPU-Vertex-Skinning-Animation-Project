#pragma once
#include <d3d11.h>


class ClearPass
{
public:
	ClearPass(ID3D11Device* pDevice, ID3D11RenderTargetView* pRTV, ID3D11DepthStencilView* pDSV);
	~ClearPass();

	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	void Render();
	void Destroy();

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11RenderTargetView* mRTV;
	ID3D11DepthStencilView* mDSV;
	float mClearColor[4];

	bool m_Enabled;
};