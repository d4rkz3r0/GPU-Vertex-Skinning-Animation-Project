#include "TransparentPass.h"
#include "PipelineState.h"
#include "RendererHelpers/AnimatedMesh.h"
#include "../../../Shared/SharedUtils.h"
#include "../../AppHelpers/Cameras/Camera.h"

unsigned int TransparentPass::gCurrentObject = 0;
float TransparentPass::transparentMeshTotalTime = 0.0f;

TransparentPass::TransparentPass(ID3D11Device* pDevice, PipelineState* pipelineState, Camera& camera, RasterizerStateManager* rasterManager) :
	mDevice(pDevice), mRasterizerStateManager(rasterManager), mEnabled(true), mPipeline(pipelineState), mCamera(&camera)
{
	mDevice->GetImmediateContext(&mDeviceContext);
}

TransparentPass::~TransparentPass()
{
	Destroy();
}

bool TransparentPass::IsEnabled() const
{
	return mEnabled;
}


void TransparentPass::PreRender()
{
	if (mPipeline)
	{
		mPipeline->BindStates();
	}
}

void TransparentPass::Render(float deltaTime)
{
	transparentMeshTotalTime += deltaTime;
	
	for (auto tGeometry : mTransparentGeometry)
	{
		if (tGeometry != nullptr && tGeometry->IsEnabled())
		{
			UpdateSortDistances();
			Sort();
			mDeviceContext->RSSetState(mRasterizerStateManager->GetRasterizerState(RasterizerStateManager::RasterStateType::DefaultCCWRasterState));
			tGeometry->Draw(transparentMeshTotalTime);
			mDeviceContext->RSSetState(mRasterizerStateManager->GetRasterizerState(RasterizerStateManager::RasterStateType::CWCullRasterState));
			tGeometry->Draw(transparentMeshTotalTime);
			gCurrentObject++;
		}
	}
}

void TransparentPass::PostRender()
{
	if (mPipeline)
	{
		mPipeline->UnBindStates();
	}
	gCurrentObject = 0;
}

void TransparentPass::SetEnabled(bool enabled)
{
	mEnabled = enabled;
}

void TransparentPass::Destroy()
{
	DeleteObject(mRasterizerStateManager);
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
	DeleteObject(mPipeline);

	mTransparentGeometry.clear();
}

//Can be extended...
void TransparentPass::UpdateSortDistances()
{
	if (gCurrentObject >= mTransparentGeometry.size())
	{
		gCurrentObject = 0;
	}

	if (gCurrentObject == 0)
	{
		XMVECTOR tGeoPosition = XMVectorZero();
		XMVector3TransformCoord(tGeoPosition, mTransparentGeometry.at(0)->GetWorld());
		XMVECTOR result = XMVector3Length(XMVectorSubtract(tGeoPosition, mCamera->GetPositionXM()));
		mTransparentGeometryDistances.at(0) = XMVectorGetX(result);
		return;
	}

	if (gCurrentObject == 1)
	{
		XMVECTOR tGeoPosition = XMVectorZero();
		XMVector3TransformCoord(tGeoPosition, mTransparentGeometry.at(1)->GetWorld());
		XMVECTOR result = XMVector3Length(XMVectorSubtract(tGeoPosition, mCamera->GetPositionXM()));
		mTransparentGeometryDistances.at(1) = XMVectorGetX(result);
		return;
	}
}


//Can be extended...
void TransparentPass::Sort()
{
	if (mTransparentGeometryDistances.at(0) < mTransparentGeometryDistances.at(1))
	{
		//Swap
		XMMATRIX tempMatrix = mTransparentGeometry.at(0)->GetWorld();
		mTransparentGeometry.at(0)->SetWorld(mTransparentGeometry.at(1)->GetWorld());
		mTransparentGeometry.at(1)->SetWorld(tempMatrix);
	}
}


void TransparentPass::SetGeometry(vector<AnimatedMesh*> TransparentGeometry)
{
	mTransparentGeometry.resize(TransparentGeometry.size());
	mTransparentGeometryDistances.resize(TransparentGeometry.size());
	mTransparentGeometry = TransparentGeometry;
}