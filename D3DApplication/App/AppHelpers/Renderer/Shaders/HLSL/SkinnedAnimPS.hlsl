#define MAX_BONES 64

//Modulation Functions
float3 get_vector_color_contribution(float4 light, float3 color)
{
	return light.rgb * light.a * color;
}

float3 get_scalar_color_contribution(float4 light, float color)
{
	return light.rgb * light.a * color;
}

cbuffer CBufferPerFrame : register(b8)
{
	float4 AmbientColor = { 1.0f, 1.0f, 1.0f, 0.0f };
	float4 LightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float3 LightPosition = { 0.0f, 0.0f, 0.0f };
	float LightRadius = 10.0f;
	float3 CameraPosition;
}

cbuffer CBufferPerObject : register(b9)
{
	float4x4 WorldViewProjection : WORLDVIEWPROJECTION;
	float4x4 World : WORLD;
	float4 SpecularColor : SPECULAR = { 1.0f, 1.0f, 1.0f, 1.0f };
	float SpecularPower : SPECULARPOWER = 25.0f;
}

cbuffer CBufferSkinning : register(b10)
{
	float4x4 BoneTransforms[MAX_BONES];
}

Texture2D mColorMap   : register(t8);
SamplerState mSampler : register(s2);

struct VS_INPUT
{
	float4 ObjectPosition : POSITION;
	float2 TextureCoordinate : TEXCOORD;
	float3 Normal : NORMAL;
	uint4 BoneIndices : BONEINDICES;
	float4 BoneWeights : WEIGHTS;
};

struct VS_OUTPUT
{
	float4 Position : SV_Position;
	float3 Normal : NORMAL;
	float2 TextureCoordinate : TEXCOORD0;
	float3 WorldPosition : TEXCOORD1;
	float Attenuation : TEXCOORD2;
};

float4 main(VS_OUTPUT IN) : SV_Target
{
	float4 OUT = (float4)0;

	float3 lightDirection = LightPosition - IN.WorldPosition;
	lightDirection = normalize(lightDirection);

	float3 viewDirection = normalize(CameraPosition - IN.WorldPosition);

	float3 normal = normalize(IN.Normal);
	float n_dot_l = dot(normal, lightDirection);
	float3 halfVector = normalize(lightDirection + viewDirection);
	float n_dot_h = dot(normal, halfVector);

	float4 color = mColorMap.Sample(mSampler, IN.TextureCoordinate);
	float4 lightCoefficients = lit(n_dot_l, n_dot_h, SpecularPower);

	float3 ambient = get_vector_color_contribution(AmbientColor, color.rgb);
	float3 diffuse = get_vector_color_contribution(LightColor, lightCoefficients.y * color.rgb) * IN.Attenuation;
	float3 specular = get_scalar_color_contribution(SpecularColor, min(lightCoefficients.z, color.w)) * IN.Attenuation;

	OUT.rgb = ambient + diffuse + specular;
	OUT.a = 1.0f;

	return OUT;
}