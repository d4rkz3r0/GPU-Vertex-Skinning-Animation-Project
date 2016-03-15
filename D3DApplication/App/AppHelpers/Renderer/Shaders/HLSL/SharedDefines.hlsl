#define NUM_LIGHTS 8

#define POINT_LIGHT 0
#define SPOT_LIGHT 1
#define DIRECTIONAL_LIGHT 2

struct Material
{
	float4  GlobalAmbient;
	float4  AmbientColor;
	float4  EmissiveColor;
	float4  DiffuseColor;
	float4  SpecularColor;
	float4  Reflectance;
	float   Opacity;
	float   SpecularPower;
	float   IndexOfRefraction;
	bool    HasAmbientTexture;
	bool    HasEmissiveTexture;
	bool    HasDiffuseTexture;
	bool    HasSpecularTexture;
	bool    HasSpecularPowerTexture;
	bool    HasNormalTexture;
	bool    HasBumpTexture;
	bool    HasOpacityTexture;
	float   BumpIntensity;
	float   SpecularScale;
	float   AlphaThreshold;
	float2  Padding;
}; 

struct Light
{
	float4   PositionWS;
	float4   DirectionWS;
	float4   PositionVS;
	float4   DirectionVS;
	float4   Color;
	float    SpotlightAngle;
	float    Range;
	float    Intensity;
	bool    Enabled;
	bool    Selected;
	uint    Type;
	float2  Padding;
};

struct AppData
{
	float3 position : POSITION;
	float3 tangent  : TANGENT;
	float3 binormal : BINORMAL;
	float3 normal   : NORMAL;
	float2 texCoord : TEXCOORD0;
};

cbuffer PerObject : register(b0)
{
	float4x4 ModelViewProjection;
	float4x4 ModelView;
}

cbuffer Material : register(b2)
{
	Material Mat;
};

Texture2D AmbientTexture        : register(t0);
Texture2D EmissiveTexture       : register(t1);
Texture2D DiffuseTexture        : register(t2);
Texture2D SpecularTexture       : register(t3);
Texture2D SpecularPowerTexture  : register(t4);
Texture2D NormalTexture         : register(t5);
Texture2D BumpTexture           : register(t6);
Texture2D OpacityTexture        : register(t7);

//StructuredBuffer<Light> Lights : register(t8);

sampler LinearRepeatSampler     : register(s0);
sampler LinearClampSampler      : register(s1);

struct VertexShaderOutput
{
	float3 positionVS   : TEXCOORD0;    // View space position.
	float2 texCoord     : TEXCOORD1;    // Texture coordinate
	float3 tangentVS    : TANGENT;      // View space tangent.
	float3 binormalVS   : BINORMAL;     // View space binormal.
	float3 normalVS     : NORMAL;       // View space normal.
	float4 position     : SV_POSITION;  // Clip space position.
};


float3 ExpandNormal(float3 n)
{
	return n * 2.0f - 1.0f;
}

struct LightingResult
{
	float4 Diffuse;
	float4 Specular;
};

float4 DoNormalMapping(float3x3 TBN, Texture2D tex, sampler s, float2 uv)
{
	float3 normal = tex.Sample(s, uv).xyz;
	normal = ExpandNormal(normal);

	normal = mul(normal, TBN);
	return normalize(float4(normal, 0));
}

float4 DoBumpMapping(float3x3 TBN, Texture2D tex, sampler s, float2 uv, float bumpScale)
{
	float height_00 = tex.Sample(s, uv).r * bumpScale;
	float height_10 = tex.Sample(s, uv, int2(1, 0)).r * bumpScale;
	float height_01 = tex.Sample(s, uv, int2(0, 1)).r * bumpScale;

	float3 p_00 = { 0, 0, height_00 };
	float3 p_10 = { 1, 0, height_10 };
	float3 p_01 = { 0, 1, height_01 };

	float3 normal = cross(normalize(p_10 - p_00), normalize(p_01 - p_00));
	normal = mul(normal, TBN);

	return float4(normal, 0);
}

float4 DoDiffuse(Light light, float4 L, float4 N)
{
	float NdotL = max(dot(N, L), 0);
	return light.Color * NdotL;
}

float4 DoSpecular(Light light, Material material, float4 V, float4 L, float4 N)
{
	float4 R = normalize(reflect(-L, N));
	float RdotV = max(dot(R, V), 0);

	return light.Color * pow(RdotV, material.SpecularPower);
}

float DoAttenuation(Light light, float d)
{
	return 1.0f - smoothstep(light.Range * 0.75f, light.Range, d);
}

float DoSpotCone(Light light, float4 L)
{
	float minCos = cos(radians(light.SpotlightAngle));
	float maxCos = lerp(minCos, 1, 0.5f);
	float cosAngle = dot(light.DirectionVS, -L);
	return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoPointLight(Light light, Material mat, float4 V, float4 P, float4 N)
{
	LightingResult result;

	float4 L = light.PositionVS - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);

	result.Diffuse = DoDiffuse(light, L, N) * attenuation * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * attenuation * light.Intensity;

	return result;
}

LightingResult DoDirectionalLight(Light light, Material mat, float4 V, float4 P, float4 N)
{
	LightingResult result;

	float4 L = normalize(-light.DirectionVS);

	result.Diffuse = DoDiffuse(light, L, N) * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * light.Intensity;

	return result;
}

LightingResult DoSpotLight(Light light, Material mat, float4 V, float4 P, float4 N)
{
	LightingResult result;

	float4 L = light.PositionVS - P;
	float distance = length(L);
	L = L / distance;

	float attenuation = DoAttenuation(light, distance);
	float spotIntensity = DoSpotCone(light, L);

	result.Diffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity * light.Intensity;
	result.Specular = DoSpecular(light, mat, V, L, N) * attenuation * spotIntensity * light.Intensity;

	return result;
}

LightingResult DoLighting(StructuredBuffer<Light> lights, Material mat, float4 eyePos, float4 P, float4 N)
{
	float4 V = normalize(eyePos - P);

	LightingResult totalResult = (LightingResult)0;

	[unroll]
	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		LightingResult result = (LightingResult)0;

		if (!lights[i].Enabled) continue;
		if (lights[i].Type != DIRECTIONAL_LIGHT && length(lights[i].PositionVS - P) > lights[i].Range) continue;

		switch (lights[i].Type)
		{
		case DIRECTIONAL_LIGHT:
		{
			result = DoDirectionalLight(lights[i], mat, V, P, N);
		}
		break;
		case POINT_LIGHT:
		{
			result = DoPointLight(lights[i], mat, V, P, N);
		}
		break;
		case SPOT_LIGHT:
		{
			result = DoSpotLight(lights[i], mat, V, P, N);
		}
		break;
		}
		totalResult.Diffuse += result.Diffuse;
		totalResult.Specular += result.Specular;
	}

	return totalResult;
}