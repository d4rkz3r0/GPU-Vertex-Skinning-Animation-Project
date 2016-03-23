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

//Normal Mapping
float3 ComputeBumpedNormal(float3 normalMapSample, float3 pixelNormal, float3 pixelTangent)
{
	//Bring Normal Maps Sampled Vec to Scale
	float3 textureNormal = 2.0f * normalMapSample - 1.0f;
	//Build TBN Matrix
	float3 N = textureNormal; //Normal
	float3 T = normalize(pixelTangent - dot(pixelTangent, N) * N); //BiTangent
	float3 B = cross(N, T); //BiNormal

	float3x3 TBN = float3x3(T, B, N);

	//Sampled Normal, Tangent Space->World Space
	float3 bumpedNormal = mul(textureNormal, TBN);

	return bumpedNormal;
}

//Directional Lighting
void DirectionalLightFactors(in float3 pixelNormal, in float3 pixelToCameraDir, in float4 textureDiffuse, out float4 oAmbient, out float4 oDiffuse, out float4 oSpecular)
{
	//Reset Values
	oAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	oDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	oSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//Flip Direction For Dot Product
	float3 pixel2LightDirection = -float3(0.0f, -0.75, 0.0f); //dir light direction

	//Ambient Component
	oAmbient = gAmbientColor * float4(0.3f, 0.3f, 0.3f, 1.0f); //dir light ambient

	float diffuseFactor = dot(pixel2LightDirection, pixelNormal);
	[flatten] //Avoid Dynamic Branching 
	if (diffuseFactor > 0.0f)
	{
		//reflect, calcs reflection vector given the incident vector and a surface normal
		float3 reflectionVector = reflect(-pixel2LightDirection, pixelNormal);
		float specularFactor = pow(max(dot(reflectionVector, pixelToCameraDir), 0.0f), gSpecularPower); //256.0f??

		//Diffuse and Specular Components
		oDiffuse = diffuseFactor * textureDiffuse * float4(1.0f, 1.0f, 1.0f, 1.0f); //White
		oSpecular = specularFactor * gSpecularPower * float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}

struct VS_OUTPUT
{
	float4 PositionHClip  : SV_Position;
	float4 PositionWorld : PositionW;
	float2 TexCoords     : TexCoord0;
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

	//Init
	float4 FinalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 FinalAmbientFactor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 FinalDiffuseFactor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 FinalSpecularFactor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 tempAmbientFactor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 tempDiffuseFactor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 tempSpecularFactor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 cameraPosition = float4(0.0f, 10.0f, -45.0f, 1.0f);
	float3 Pixel2CameraDirection = normalize(cameraPosition.xyz - FromRasterizer.PositionWorld.xyz); //camera pos needed
	float4 DiffuseMapSample = mColorMap.Sample(mSampler, FromRasterizer.TexCoords);
	float3 Normal = normalize(FromRasterizer.NormalWorld);
	float3 Tangent = normalize(FromRasterizer.TangentWorld);
	float3 Bitangent = normalize(FromRasterizer.BiTangentWorld);
	float3 NormalMapSample = mNormalMap.Sample(mSampler, FromRasterizer.TexCoords).rgb;
	float3 PixelNormVec = normalize(FromRasterizer.NormalWorld);

	//Normal Mapping
	float3 BumpedNormal = ComputeBumpedNormal(NormalMapSample, PixelNormVec, FromRasterizer.TangentWorld);
	
	//Directional Light
	DirectionalLightFactors(BumpedNormal, Pixel2CameraDirection, DiffuseMapSample, tempAmbientFactor, tempDiffuseFactor, tempSpecularFactor);
	FinalAmbientFactor += tempAmbientFactor;
	FinalDiffuseFactor += tempDiffuseFactor;
	FinalSpecularFactor += tempSpecularFactor;

	//Combine
	FinalColor = FinalAmbientFactor + FinalDiffuseFactor + FinalSpecularFactor;
	FinalColor.a = gDiffuseColor.a;
	return FinalColor;
}