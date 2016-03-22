#include "OpaquePass.h"
#include "../../../Shared/SharedUtils.h"

float OpaquePass::opaqueMeshTotalTime = 0.0f;

OpaquePass::OpaquePass(ID3D11Device* pDevice, PipelineState* pipelineState) : mDevice(pDevice), mEnabled(true), mPipeline(pipelineState)
{
	mDevice->GetImmediateContext(&mDeviceContext);
}

OpaquePass::~OpaquePass()
{
	Destroy();
}

bool OpaquePass::IsEnabled() const
{
	return mEnabled;
}


void OpaquePass::PreRender()
{
	if (mPipeline)
	{
		mPipeline->BindStates();
	}
}

void OpaquePass::Render(float deltaTime)
{
	opaqueMeshTotalTime += deltaTime;

	for (auto oGeometry : mOpaqueGeometry)
	{
		if (oGeometry != nullptr && oGeometry->IsEnabled())
		{
			oGeometry->Draw(opaqueMeshTotalTime);
		}
	}
}

void OpaquePass::PostRender()
{
	if (mPipeline)
	{
		mPipeline->UnBindStates();
	}
}

void OpaquePass::SetEnabled(bool enabled)
{
	mEnabled = enabled;
}

void OpaquePass::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
	DeleteObject(mPipeline);

	mOpaqueGeometry.clear();
}

void OpaquePass::SetGeometry(vector<AnimatedMesh*> OpaqueGeometry)
{
	mOpaqueGeometry.resize(OpaqueGeometry.size());
	mOpaqueGeometry = OpaqueGeometry;
}