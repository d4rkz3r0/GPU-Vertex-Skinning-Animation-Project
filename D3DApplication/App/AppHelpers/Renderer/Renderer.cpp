#include "Renderer.h"
#include "../Renderer/PipelineState.h"
#include "RendererHelpers/BlendStateManager.h"
#include "RendererHelpers/Shader.h"
#include "RendererHelpers/SamplerStateManager.h"
#include "RendererHelpers/RasterizerStateManager.h"
#include "RendererHelpers/DepthStencilStateManager.h"
#include "TransparentPass.h"
#include "OpaquePass.h"
#include "ClearPass.h"
#include "Technique.h"
#include "../../../Shared/SharedUtils.h"


Renderer::Renderer(ID3D11Device* theDevice, Camera& camera, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV)
	: mDevice(theDevice), mRenderTargetView(backBufferRTV), mDepthStencilView(backBufferDSV), mCamera(&camera)
{
	mDevice->GetImmediateContext(&mDeviceContext);

	//Rev Up Those Fryers
	Initialize();
}

Renderer::~Renderer()
{
	Destroy();
}

void Renderer::Initialize()
{
	//-Initialize Models-//
	mOpaqueGeometry.push_back(new AnimatedMesh(mDevice, *mCamera));
	mOpaqueGeometry.push_back(new AnimatedMesh(mDevice, *mCamera));
	mOpaqueGeometry.push_back(new AnimatedMesh(mDevice, *mCamera));
	mOpaqueGeometry.push_back(new AnimatedMesh(mDevice, *mCamera));

	mTransparentGeometry.push_back(new AnimatedMesh(mDevice, *mCamera));
	mTransparentGeometry.push_back(new AnimatedMesh(mDevice, *mCamera));

	//-Load Models-//
	mOpaqueGeometry.at(0)->Load("Teddy_Idle.dae");
	mOpaqueGeometry.at(0)->SetModelOrientation(-60.0f, 0.0f, 10.0f);
	mOpaqueGeometry.at(1)->Load("Teddy_Run.dae");
	mOpaqueGeometry.at(1)->SetModelOrientation(0.0f, 0.0f, 10.0f);
	mOpaqueGeometry.at(2)->Load("Teddy_Attack1.dae");
	mOpaqueGeometry.at(2)->SetModelOrientation(-20.0f, 0.0f, 10.0f);
	mOpaqueGeometry.at(3)->Load("Teddy_Attack2.dae");
	mOpaqueGeometry.at(3)->SetModelOrientation(-40.0f, 0.0f, 10.0f);
	
	mTransparentGeometry.at(0)->Load("Teddy_Idle.dae");
	mTransparentGeometry.at(0)->SetModelOrientation(20.0f, 0.0f, 10.0f);
	mTransparentGeometry.at(1)->Load("Teddy_Run.dae");
	mTransparentGeometry.at(1)->SetModelOrientation(40.0f, 0.0f, 10.0f);

	//Create Pipeline Parameters for Pipeline States via Manager Initialization
	mDepthStencilStateManager = new DepthStencilStateManager(mDevice);
	mRasterizerStateManager = new RasterizerStateManager(mDevice);
	mSamplerStateManager = new SamplerStateManager(mDevice);
	mBlendStateManager = new BlendStateManager(mDevice);

	//Load & Compile Shaders
	mAnimationVertexShader = new Shader(mDevice);
	mAnimationVertexShader->LoadShader(Shader::VertexShader, 1);
	mAnimationPixelShader = new Shader(mDevice);
	mAnimationPixelShader->LoadShader(Shader::PixelShader, 1);

	//-Initialize PipelineStates-//
	mOpaquePipeLineState = new PipelineState(mDevice);
	mOpaquePipeLineState->SetShader(Shader::VertexShader, mAnimationVertexShader);
	mOpaquePipeLineState->SetShader(Shader::PixelShader, mAnimationPixelShader);
	mOpaquePipeLineState->SetRasterizerState(mRasterizerStateManager->GetRasterizerState(RasterizerStateManager::RasterStateType::NoCullRasterState));
	mOpaquePipeLineState->SetBlendState(mBlendStateManager->GetBlendState(BlendStateManager::BlendStateType::OpaqueBlendState), mBlendStateManager->GetOpaqueBlendFactor(), mBlendStateManager->GetSampleMask());
	mOpaquePipeLineState->SetDepthStencilState(mDepthStencilStateManager->GetDepthStencilState(DepthStencilStateManager::DepthStencilStateType::DepthStencilStateLessEqual), mDepthStencilStateManager->GetDepthStencilStateRef(DepthStencilStateManager::DepthStencilStateType::DepthStencilStateLessEqual));
	
	mTransparentPipeLineState = new PipelineState(mDevice);
	mTransparentPipeLineState->SetShader(Shader::VertexShader, mAnimationVertexShader);
	mTransparentPipeLineState->SetShader(Shader::PixelShader, mAnimationPixelShader);
	mTransparentPipeLineState->SetBlendState(mBlendStateManager->GetBlendState(BlendStateManager::BlendStateType::TransparencyBlendState), mBlendStateManager->GetTransparentBlendFactor(), mBlendStateManager->GetSampleMask());
	mTransparentPipeLineState->SetDepthStencilState(mDepthStencilStateManager->GetDepthStencilState(DepthStencilStateManager::DepthStencilStateType::DepthStencilStateLessEqual), mDepthStencilStateManager->GetDepthStencilStateRef(DepthStencilStateManager::DepthStencilStateType::DepthStencilStateLessEqual));

	//-Create Render Passes-//
	mClearPass = new ClearPass(mDevice, mRenderTargetView, mDepthStencilView);
	mOpaquePass = new OpaquePass(mDevice, mOpaquePipeLineState);
	mOpaquePass->SetGeometry(mOpaqueGeometry);
	mTransparentPass = new TransparentPass(mDevice, mTransparentPipeLineState, *mCamera, mRasterizerStateManager);
	mTransparentPass->SetGeometry(mTransparentGeometry);

	//-Build Technique-//
	mSkinnedAnimationForwardRenderingTechnique = new Technique();
	mSkinnedAnimationForwardRenderingTechnique->AddClearPass(mClearPass);
	mSkinnedAnimationForwardRenderingTechnique->AddOpaquePass(mOpaquePass);
	mSkinnedAnimationForwardRenderingTechnique->AddTransparentPass(mTransparentPass);
}



void Renderer::Draw(float deltaTime)
{
	mSkinnedAnimationForwardRenderingTechnique->Render(deltaTime);
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

}


void Renderer::Destroy()
{
	DeleteObject(mAnimationVertexShader);
	DeleteObject(mAnimationPixelShader);

	DeleteObject(mOpaquePipeLineState);
	DeleteObject(mTransparentPipeLineState);

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