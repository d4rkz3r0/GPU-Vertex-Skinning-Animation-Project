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

void TransparentPass::UpdateSortDistances()
{
	if (gCurrentObject != 2)
	{
		XMVECTOR tGeoPosition = XMVectorZero();
		tGeoPosition = XMVector3TransformCoord(tGeoPosition, mTransparentGeometry.at(gCurrentObject)->GetWorld());

		//Drawn Out Dot Product
		float distX = XMVectorGetX(tGeoPosition) - XMVectorGetX(mCamera->GetPositionXM());
		float distY = XMVectorGetY(tGeoPosition) - XMVectorGetY(mCamera->GetPositionXM());
		float distZ = XMVectorGetZ(tGeoPosition) - XMVectorGetZ(mCamera->GetPositionXM());
		mTransparentGeometryDistances.at(gCurrentObject) = distX * distX + distY * distY + distZ * distZ;
	}

	if (gCurrentObject == 2)
	{
		for (size_t i = 0; i < mTransparentGeometryDistances.size(); i++)
		{
			if (mTransparentGeometryDistances.at(0) < mTransparentGeometryDistances.at(1))
			{
				//Swap
				XMMATRIX tempMatrix = mTransparentGeometry.at(0)->GetWorld();
				mTransparentGeometry.at(0)->SetWorld(mTransparentGeometry.at(1)->GetWorld());
				mTransparentGeometry.at(1)->SetWorld(tempMatrix);
			}
		}
		gCurrentObject = 0;
	}
}

void TransparentPass::SetGeometry(vector<AnimatedMesh*> TransparentGeometry)
{
	mTransparentGeometry.resize(TransparentGeometry.size());
	mTransparentGeometryDistances.resize(TransparentGeometry.size());
	mTransparentGeometry = TransparentGeometry;
}