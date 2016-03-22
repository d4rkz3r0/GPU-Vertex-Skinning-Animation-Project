#pragma once
#include <DirectXMath.h>
using namespace DirectX;


#define CB_PER_OBJECT_MATRIX_REGISTER_SLOT 0
struct cbPerObjectMatrix
{
	cbPerObjectMatrix()
	{
		ZeroMemory(this, sizeof(this));
	}
	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT4X4 mViewProjMatrix;
	XMFLOAT4X4 mInvTransWorldMatrix;
};


#define CB_PER_OBJECT_MATERIAL_REGISTER_SLOT 1
struct cbPerObjectMaterial
{
	cbPerObjectMaterial()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4   mSpecularColor;
	XMFLOAT4   mDiffuseColor;
	XMFLOAT4   mAmbientColor;
	XMFLOAT4   mEmissiveColor;
	float	   mSpecularPower;
	XMFLOAT3   mPadding;
};


#define CB_PER_OBJECT_SKINNING_REGISTER_SLOT 2
#define MAX_BONES 64
struct cbPerObjectSkinningData
{
	cbPerObjectSkinningData()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4X4 BoneTransforms[MAX_BONES];
};