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
	XMFLOAT4X4 mWorldViewProj;
	XMFLOAT4X4 mWorldMatrix;
};


#define CB_PER_OBJECT_MATERIAL_REGISTER_SLOT 2
struct cbPerObjectMaterial
{
	cbPerObjectMaterial()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4   m_GlobalAmbient;
	XMFLOAT4   m_AmbientColor;
	XMFLOAT4   m_EmissiveColor;
	XMFLOAT4   m_DiffuseColor;
	XMFLOAT4   m_SpecularColor;
	XMFLOAT4   m_Reflectance;
	float       m_Opacity;
	float       m_SpecularPower;
	float       m_IndexOfRefraction;
	uint32_t    m_HasAmbientTexture;
	uint32_t    m_HasEmissiveTexture;
	uint32_t    m_HasDiffuseTexture;
	uint32_t    m_HasSpecularTexture;
	uint32_t    m_HasSpecularPowerTexture;
	uint32_t    m_HasNormalTexture;
	uint32_t    m_HasBumpTexture;
	uint32_t    m_HasOpacityTexture;
	float       m_BumpIntensity;
	float       m_SpecularScale;
	float       m_AlphaThreshold;
	XMFLOAT2  m_Padding;
};


#define CB_PER_OBJECT_MATRIX_TEMP_REGISTER_SLOT 9
struct cbPerObjectMatrixTemp
{
	cbPerObjectMatrixTemp()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4X4 WorldViewProjection;
	XMFLOAT4X4 World;
	XMFLOAT4 SpecularColor;
	float SpecularPower;
};


#define CB_PER_OBJECT_SKINNING_REGISTER_SLOT 10
#define MAX_BONES 64
struct cbPerObjectSkinningData
{
	XMFLOAT4X4 BoneTransforms[MAX_BONES];
};