#include "SharedDefines.hlsl"

float4 PS_light(VertexShaderOutput IN) : SV_TARGET
{
	float4 N = normalize(float4(IN.normalVS, 0));

	return float4((Mat.DiffuseColor * saturate(N.z)).rgb, Mat.Opacity);
}