#include "TransparentPass.h"
#include "../../../Shared/SharedUtils.h"
//#include "RendererHelpers/Model.h"
//#include "RendererHelpers/Mesh.h"

TransparentPass::TransparentPass(ID3D11Device* pDevice, PipelineState* pipelineState) : mDevice(pDevice), m_Enabled(true), m_Pipeline(pipelineState)
{
	
}

TransparentPass::~TransparentPass()
{
	Destroy();
}

bool TransparentPass::IsEnabled() const
{
	return m_Enabled;
}


void TransparentPass::PreRender()
{
	if (m_Pipeline)
	{
		m_Pipeline->BindStates();
	}
}

void TransparentPass::Render(Model& model)
{
	//if (model != nullptr && model->IsTransparent())
	//{
	//   for(auto aMesh : model.mMeshes)
	//			aMesh.Render(); ->Draw
	//	
	//}
}

void TransparentPass::PostRender()
{
	if (m_Pipeline)
	{
		m_Pipeline->UnBindStates();
	}
}

void TransparentPass::SetEnabled(bool enabled)
{
	m_Enabled = enabled;
}

void TransparentPass::Destroy()
{
	ReleaseObject(mDevice);
	DeleteObject(m_Pipeline);
}