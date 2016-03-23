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

struct VS_INPUT
{
	float3 PositionLocal : POSITION;
	float2 TexCoordsLocal: TEXCOORD0;
	float3 NormalLocal   : NORMAL;
	float3 TangentLocal  : TANGENT;
	float4 BonesIndices	 : BONE;
	float4 BoneWeights	 : BLENDWEIGHT;
};

struct VS_OUTPUT
{
	float4 PositionHClip  : SV_Position;
	float4 PositionWorld : PositionW;
	float2 TexCoords     : TexCoord0;
	float3 NormalWorld   : Normal;
	float3 TangentWorld  : Tangent;
	float3 BiTangentWorld: Bitangent;
};


VS_OUTPUT main(VS_INPUT FromApp)
{
	VS_OUTPUT OUT = (VS_OUTPUT)0;

	//Calc It
	float4x4 skinningMatrix;
	skinningMatrix  = FromApp.BoneWeights.x * gBoneTransforms[FromApp.BonesIndices.x];
	skinningMatrix += FromApp.BoneWeights.y * gBoneTransforms[FromApp.BonesIndices.y];
	skinningMatrix += FromApp.BoneWeights.z * gBoneTransforms[FromApp.BonesIndices.z];
	skinningMatrix += FromApp.BoneWeights.w * gBoneTransforms[FromApp.BonesIndices.w];

	//Apply It
	float4 vAnimatedPosition = mul(float4(FromApp.PositionLocal, 1.0f), skinningMatrix);
	float4 vAnimatedTangent  = mul(float4(FromApp.TangentLocal, 0.0f), skinningMatrix);
	float4 vAnimatedNormal   = mul(float4(FromApp.NormalLocal, 0.0f), skinningMatrix);

	//Transform Between Coordinate Systems
	OUT.PositionHClip = mul(mul(vAnimatedPosition, gWorldMatrix), gViewProjectionMatrix);
	OUT.PositionWorld = mul(vAnimatedPosition, gWorldMatrix);
	OUT.NormalWorld = mul(vAnimatedNormal.xyz, (float3x3)gInvTransposeObjectToWorld);
	OUT.TangentWorld = mul(vAnimatedTangent.xyz, (float3x3)gInvTransposeObjectToWorld);
	OUT.BiTangentWorld = cross(OUT.NormalWorld, OUT.TangentWorld);

	//Pass-Through
	OUT.TexCoords = FromApp.TexCoordsLocal;

	return OUT;
}