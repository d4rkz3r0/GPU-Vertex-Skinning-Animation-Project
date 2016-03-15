#include "OpaquePass.h"
#include "RendererHelpers/Material.h"
#include "../../../Shared/SharedUtils.h"
//#include "RendererHelpers/Model.h"
//#include "RendererHelpers/Mesh.h"

OpaquePass::OpaquePass(ID3D11Device* pDevice, PipelineState* pipelineState) : mDevice(pDevice), m_Enabled(true), m_Pipeline(pipelineState)
{
	
}

OpaquePass::~OpaquePass()
{
	Destroy();
}

bool OpaquePass::IsEnabled() const
{
	return m_Enabled;
}


void OpaquePass::PreRender()
{
	if (m_Pipeline)
	{
		m_Pipeline->BindStates();
	}
}

void OpaquePass::Render(Model& model)
{
	/*if (model != nullptr && model->IsOpaque())
	{
	   for(auto aMesh : model.mMeshes)
				aMesh.Render();
							   1) ->Set Topology
							   2) ->BindBuffers
							   3) ->Send Materials(Renderer covers this)
							   4) ->DrawIndexed()
	}*/
}

void OpaquePass::PostRender()
{
	if (m_Pipeline)
	{
		m_Pipeline->UnBindStates();
	}
}

void OpaquePass::SetEnabled(bool enabled)
{
	m_Enabled = enabled;
}

void OpaquePass::Destroy()
{
	ReleaseObject(mDevice);
	//m_PerObjectConstantBuffer.ShutDown();
	DeleteObject(m_Pipeline);
}