#pragma once
#include <DirectXMath.h>
using namespace DirectX;
#define MAX_BONES 64

struct cbPerSkinnedObject
{
	XMFLOAT4X4 mSkinnedViewProj;
	XMFLOAT4X4 mSkinnedMatrices[NUM_BONES];
};

struct cbPerObjectMatrix
{
	cbPerObjectMatrix()
	{
		ZeroMemory(this, sizeof(this));
	}
	XMFLOAT4X4 mWorldViewProj;
	DirectX::XMFLOAT4X4 mWorldMatrix;
};


#define MAX_BONES 64
#define CB_PER_OBJECT_REGISTER_SLOT 9
