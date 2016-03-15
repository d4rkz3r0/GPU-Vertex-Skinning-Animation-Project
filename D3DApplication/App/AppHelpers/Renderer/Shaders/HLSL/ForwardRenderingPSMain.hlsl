#include "SharedDefines.hlsl"

[earlydepthstencil]
float4 PS_main(VertexShaderOutput IN) : SV_TARGET
{

float4 eyePos = { 0, 0, 0, 1 };
Material mat = Mat;

float4 diffuse = mat.DiffuseColor;
if (mat.HasDiffuseTexture)
{
	float4 diffuseTex = DiffuseTexture.Sample(LinearRepeatSampler, IN.texCoord);
	if (any(diffuse.rgb))
	{
		diffuse *= diffuseTex;
	}
	else
	{
		diffuse = diffuseTex;
	}
}

float alpha = diffuse.a;
if (mat.HasOpacityTexture)
{
	
	alpha = OpacityTexture.Sample(LinearRepeatSampler, IN.texCoord).r;
}

float4 ambient = mat.AmbientColor;
if (mat.HasAmbientTexture)
{
	float4 ambientTex = AmbientTexture.Sample(LinearRepeatSampler, IN.texCoord);
	if (any(ambient.rgb))
	{
		ambient *= ambientTex;
	}
	else
	{
		ambient = ambientTex;
	}
}

ambient *= mat.GlobalAmbient;

float4 emissive = mat.EmissiveColor;
if (mat.HasEmissiveTexture)
{
	float4 emissiveTex = EmissiveTexture.Sample(LinearRepeatSampler, IN.texCoord);
	if (any(emissive.rgb))
	{
		emissive *= emissiveTex;
	}
	else
	{
		emissive = emissiveTex;
	}
}

if (mat.HasSpecularPowerTexture)
{
	mat.SpecularPower = SpecularPowerTexture.Sample(LinearRepeatSampler, IN.texCoord).r * mat.SpecularScale;
}

float4 N;


if (mat.HasNormalTexture)
{
	float3x3 TBN = float3x3(normalize(IN.tangentVS),
		normalize(IN.binormalVS),
		normalize(IN.normalVS));

	N = DoNormalMapping(TBN, NormalTexture, LinearRepeatSampler, IN.texCoord);
}

else if (mat.HasBumpTexture)
{
	float3x3 TBN = float3x3(normalize(IN.tangentVS),
		normalize(-IN.binormalVS),
		normalize(IN.normalVS));

	N = DoBumpMapping(TBN, BumpTexture, LinearRepeatSampler, IN.texCoord, mat.BumpIntensity);
}

else
{
	N = normalize(float4(IN.normalVS, 0));
}

float4 P = float4(IN.positionVS, 1);

return float4(1.0f, 1.0f, 1.0f, 1.0f);

}