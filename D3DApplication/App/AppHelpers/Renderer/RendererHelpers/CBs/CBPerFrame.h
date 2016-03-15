#pragma once
#include <DirectXMath.h>
using namespace DirectX;


#define CB_PER_FRAME_LIGHT_CAMERA_SLOT 8
struct cbPerFrameLightCameraData
{
	cbPerFrameLightCameraData()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 AmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	XMFLOAT4 LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 LightPosition = { 0.0f, 0.0f, 0.0f };
	float LightRadius = 10.0f;
	XMFLOAT3 CameraPosition;
};