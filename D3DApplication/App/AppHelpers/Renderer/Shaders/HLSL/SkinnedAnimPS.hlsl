#define MAX_BONES 64

cbuffer CBufferPerObjectMatrixInfo : register(b0)
{
	float4x4	gWorldMatrix;
	float4x4	gViewProjectionMatrix;
	float4x4	gInvTransposeObjectToWorld;
}

cbuffer CBufferPerObjectMaterialInfo : register(b1)
{
	float4 gSpecularColor;
	float4 gDiffuseColor;
	float4 gAmbientColor;
	float4 gEmissiveColor;
	float  gSpecularPower;
	float  gPadding[3];
};

cbuffer CBufferPerObjectSkinningInfo : register(b2)
{
	float4x4 gBoneTransforms[MAX_BONES];
}

struct VS_OUTPUT
{
	float4 PositionWorld : SV_Position;
	float2 TexCoords  : TexCoord0;
	float3 NormalWorld   : Normal;
	float3 TangentWorld  : Tangent;
	float3 BiTangentWorld: Bitangent;
};


Texture2D mColorMap     : register(t0);
Texture2D mNormalMap    : register(t1);
SamplerState mSampler   : register(s0);


float4 main(VS_OUTPUT FromRasterizer) : SV_Target
{
	float4 OUTCOLOR = (float4)0;

	OUTCOLOR = mColorMap.Sample(mSampler, FromRasterizer.TexCoords);
	float3 Normal = normalize(FromRasterizer.NormalWorld);
	float3 Tangent = normalize(FromRasterizer.TangentWorld);
	float3 Bitangent = normalize(FromRasterizer.BiTangentWorld);
	//mDiffuseMap.Sample(mSampler, input.texcoord).xyz * 2.0 - 1;
	//float4(normalize(Tangent * normal.x + Bitangent * normal.y + Normal * normal.z), 1);

	return OUTCOLOR;
}