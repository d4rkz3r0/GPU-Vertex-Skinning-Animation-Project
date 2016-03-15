#include "Renderer.h"
#include "RendererHelpers/CBs/CBRefs.h"
#include "RendererHelpers/PipelineState.h"
#include "RendererHelpers/BlendStateManager.h"
#include "RendererHelpers/CBs/ConstantBuffer.h"
#include "RendererHelpers/VertexBufferManager.h"
#include "RendererHelpers/SamplerStateManager.h"
#include "RendererHelpers/RasterizerStateManager.h"
#include "RendererHelpers/DepthStencilStateManager.h"
#include "RendererHelpers/StructuredBuffer.h"
#include "../../../Shared/SharedUtils.h"


Renderer::Renderer(ID3D11Device* theDevice, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV) 
	: mDevice(theDevice), mRenderTargetView(backBufferRTV), mDepthStencilView(backBufferDSV), g_pCurrentLight(nullptr), g_pLightsStructuredBuffer(nullptr)
{
	mDevice->GetImmediateContext(&mDeviceContext);

	//Prep Constant Buffers for Data
	mPerObjectMatrixData.Initialize(mDevice);
	mPerObjectMaterialData.Initialize(mDevice);

	//Prep Light Structured Buffer
	mNumLightsToMake = 8;
	//g_pLightsStructuredBuffer = new StructuredBuffer(mDevice, mData, mNumLights, mLightTypes, , ,)

	//Start Up Pipeline StateManagers
	Initialize();
}

Renderer::~Renderer()
{
	Destroy();
}

void Renderer::Initialize()
{
	//Create Pipeline Parameters for Pipeline States via Managers
	mBlendStateManager = new BlendStateManager(mDevice);
	mDepthStencilStateManager = new DepthStencilStateManager(mDevice);
	mRasterizerStateManager = new RasterizerStateManager(mDevice);
	mSamplerStateManager = new SamplerStateManager(mDevice);
	mVertexBufferManager = new VertexBufferManager;

	//Load & Compile Shaders
	g_pVertexShader = new Shader(mDevice);
	g_pVertexShader->LoadShader(Shader::VertexShader);
	g_pPixelShader = new Shader(mDevice);
	g_pPixelShader->LoadShader(Shader::PixelShader, 0);
	g_pLightPixelShader = new Shader(mDevice);
	g_pLightPixelShader->LoadShader(Shader::PixelShader, 1);

	//Create Pipeline Configurations
	g_pOpaquePipeline = new PipelineState(mDevice);
	g_pOpaquePipeline->SetShader(Shader::VertexShader, g_pVertexShader);
	g_pOpaquePipeline->SetShader(Shader::PixelShader, g_pPixelShader);
	//g_pOpaquePipeline->SetRasterizerState(mRasterizerStateManager->GetRasterizerState(RasterizerStateManager::RasterStateType::NOCULL_RasterState));
	//g_pOpaquePipeline->SetBlendState(mBlendStateManager->GetBlendState(BlendStateManager::BlendStateType::DefaultBlendState), mBlendStateManager->GetBlendFactor(), mBlendStateManager->GetSampleMask());
	//g_pOpaquePipeline->SetDepthStencilState(mDepthStencilStateManager->GetDepthStencilState(DepthStencilStateManager::DepthStencilStateType::DefaultDepthStencilState), mDepthStencilStateManager->GetDepthStencilStateRef(DepthStencilStateManager::DepthStencilStateType::DefaultDepthStencilState));
	
	g_pTransparentPipeline = new PipelineState(mDevice);
	g_pTransparentPipeline->SetShader(Shader::VertexShader, g_pVertexShader);
	g_pTransparentPipeline->SetShader(Shader::PixelShader, g_pPixelShader);
	g_pTransparentPipeline->SetBlendState(mBlendStateManager->GetBlendState(BlendStateManager::BlendStateType::AlphaBlendState), mBlendStateManager->GetBlendFactor(), mBlendStateManager->GetSampleMask());
	g_pTransparentPipeline->SetDepthStencilState(mDepthStencilStateManager->GetDepthStencilState(DepthStencilStateManager::DepthStencilStateType::DepthStencilStateNoDepth), mDepthStencilStateManager->GetDepthStencilStateRef(DepthStencilStateManager::DepthStencilStateNoDepth));
	g_pTransparentPipeline->SetRasterizerState(mRasterizerStateManager->GetRasterizerState(RasterizerStateManager::RasterStateType::NOCULL_RasterState));

	//Create Render Passes
	g_OpaquePass = new OpaquePass(mDevice, g_pOpaquePipeline);
	g_TransparentPass = new TransparentPass(mDevice, g_pTransparentPipeline);
	g_ClearPass = new ClearPass(mDevice, mRenderTargetView, mDepthStencilView);

	//Build Technique
	mForwardRenderingTechnique = new Technique();
	mForwardRenderingTechnique->AddClearPass(g_ClearPass);
	mForwardRenderingTechnique->AddOpaquePass(g_OpaquePass);
	mForwardRenderingTechnique->AddTransparentPass(g_TransparentPass);

	//Build Lights
	BuildLights(mNumLightsToMake);

	//Pre-populate Material Constant Buffer for all Forward Rendered Objects
	PopulateMaterialConstantBuffer();
}

void Renderer::PreRender()
{
	//StopWorldHunger();
	//mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
}

void Renderer::Render()
{
	mForwardRenderingTechnique->Render();
}

void Renderer::PostRender()
{
	//UnleashHellOnEarth();
	//mDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void Renderer::SaveStates()
{
	mBlendStateManager->SaveBlendState();
	mDepthStencilStateManager->SaveDepthStencilState();
	mRasterizerStateManager->SaveRasterizerState();
}

void Renderer::RestoreStates()
{
	mBlendStateManager->RestoreBlendState();
	mDepthStencilStateManager->RestoreDepthStencilState();
	mRasterizerStateManager->RestoreRasterizerState();
}


void Renderer::Update()
{
	//-Constant Buffer Updates-//
	cbPerObjectMaterial perObjMaterialCB;
	perObjMaterialCB.m_AmbientColor = mPerObjectMaterialData.Data.m_AmbientColor;
	perObjMaterialCB.m_EmissiveColor = mPerObjectMaterialData.Data.m_EmissiveColor;
	perObjMaterialCB.m_DiffuseColor = mPerObjectMaterialData.Data.m_DiffuseColor;
	perObjMaterialCB.m_SpecularColor = mPerObjectMaterialData.Data.m_SpecularColor;
	perObjMaterialCB.m_Reflectance = mPerObjectMaterialData.Data.m_Reflectance;
	perObjMaterialCB.m_Opacity = mPerObjectMaterialData.Data.m_Opacity;
	perObjMaterialCB.m_SpecularPower = mPerObjectMaterialData.Data.m_SpecularPower;
	perObjMaterialCB.m_IndexOfRefraction = mPerObjectMaterialData.Data.m_IndexOfRefraction;
	perObjMaterialCB.m_HasAmbientTexture = mPerObjectMaterialData.Data.m_HasAmbientTexture;
	perObjMaterialCB.m_HasEmissiveTexture = mPerObjectMaterialData.Data.m_HasEmissiveTexture;
	perObjMaterialCB.m_HasDiffuseTexture = mPerObjectMaterialData.Data.m_HasDiffuseTexture;
	perObjMaterialCB.m_HasSpecularTexture = mPerObjectMaterialData.Data.m_HasSpecularTexture;
	perObjMaterialCB.m_HasSpecularPowerTexture = mPerObjectMaterialData.Data.m_HasSpecularPowerTexture;
	perObjMaterialCB.m_HasNormalTexture = mPerObjectMaterialData.Data.m_HasNormalTexture;
	perObjMaterialCB.m_HasBumpTexture = mPerObjectMaterialData.Data.m_HasBumpTexture;
	perObjMaterialCB.m_HasOpacityTexture = mPerObjectMaterialData.Data.m_HasOpacityTexture;
	perObjMaterialCB.m_BumpIntensity = mPerObjectMaterialData.Data.m_BumpIntensity;
	perObjMaterialCB.m_SpecularScale = mPerObjectMaterialData.Data.m_SpecularScale;
	perObjMaterialCB.m_AlphaThreshold = mPerObjectMaterialData.Data.m_AlphaThreshold;
	mPerObjectMaterialData.Data = perObjMaterialCB;
	mPerObjectMaterialData.ApplyChanges(mDeviceContext);
	auto cMaterialBuffer = mPerObjectMaterialData.Buffer();
	mDeviceContext->VSSetConstantBuffers(2, 1, &cMaterialBuffer);
	mDeviceContext->PSSetConstantBuffers(2, 1, &cMaterialBuffer);
}

void Renderer::BuildLights(UINT numLightsToBuild)
{

}

void Renderer::UpdateLights()
{
	
}

void Renderer::UpdateNumLights()
{

}

void Renderer::Destroy()
{
	DeleteObject(g_pVertexShader);
	DeleteObject(g_pPixelShader);
	DeleteObject(g_pLightPixelShader);

	mPerObjectMatrixData.ShutDown();
	mPerObjectMaterialData.ShutDown();
	
	DeleteObject(g_pOpaquePipeline);
	DeleteObject(g_pTransparentPipeline);

	DeleteObject(mBlendStateManager);
	DeleteObject(mDepthStencilStateManager);
	DeleteObject(mRasterizerStateManager);
	DeleteObject(mSamplerStateManager);

	//Enable to track down D3D11 Memory Leaks
#if 0
	ID3D11Debug* debugDev;
	mD3DDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDev));
	debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	SAFE_RELEASE(debugDev);
#endif
}

void Renderer::PopulateMaterialConstantBuffer()
{
	//Global Currently
	mPerObjectMaterialData.Data.m_GlobalAmbient = XMFLOAT4(1.0, 1.0f, 1.0f, 1.0f);
	mPerObjectMaterialData.Data.m_AmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mPerObjectMaterialData.Data.m_EmissiveColor = XMFLOAT4();
	mPerObjectMaterialData.Data.m_DiffuseColor = XMFLOAT4();
	mPerObjectMaterialData.Data.m_SpecularColor = XMFLOAT4();
	mPerObjectMaterialData.Data.m_Reflectance = XMFLOAT4();
	mPerObjectMaterialData.Data.m_Opacity = 1.0f;
	mPerObjectMaterialData.Data.m_SpecularPower = 126.0f;
	mPerObjectMaterialData.Data.m_IndexOfRefraction = 1.5f;
	mPerObjectMaterialData.Data.m_HasAmbientTexture = false;
	mPerObjectMaterialData.Data.m_HasEmissiveTexture = false;
	mPerObjectMaterialData.Data.m_HasDiffuseTexture = false;
	mPerObjectMaterialData.Data.m_HasSpecularTexture = false;
	mPerObjectMaterialData.Data.m_HasSpecularPowerTexture = false;
	mPerObjectMaterialData.Data.m_HasNormalTexture = false;
	mPerObjectMaterialData.Data.m_HasBumpTexture = false;
	mPerObjectMaterialData.Data.m_HasOpacityTexture = false;
	mPerObjectMaterialData.Data.m_BumpIntensity = 0.0f;
	mPerObjectMaterialData.Data.m_SpecularScale = 1.0f;
	mPerObjectMaterialData.Data.m_AlphaThreshold = 0.0f;
}

//WVP Constant Buffer Update
void Renderer::SetPerObjectData(XMMATRIX& mMVP, XMMATRIX& mWorld)
{
	XMFLOAT4X4 tempMVP;
	XMFLOAT4X4 tempWorld;
	XMStoreFloat4x4(&tempMVP, mMVP);
	XMStoreFloat4x4(&tempWorld, mWorld);

	SetPerObjectData(tempMVP, tempWorld);
}

void Renderer::SetPerObjectData(XMFLOAT4X4& mMVP, XMFLOAT4X4& mWorld)
{
	cbPerObjectMatrix mPerObjectMatrixCB;
	mPerObjectMatrixCB.mWorldViewProj = mMVP;
	mPerObjectMatrixCB.mWorldMatrix = mWorld;
	mPerObjectMatrixData.Data = mPerObjectMatrixCB;
	mPerObjectMatrixData.ApplyChanges(mDeviceContext);
	auto perObjectConstantBufferData = mPerObjectMatrixData.Buffer();

	mDeviceContext->VSSetConstantBuffers(0, 1, &perObjectConstantBufferData);
	mDeviceContext->PSSetConstantBuffers(0, 1, &perObjectConstantBufferData);
}