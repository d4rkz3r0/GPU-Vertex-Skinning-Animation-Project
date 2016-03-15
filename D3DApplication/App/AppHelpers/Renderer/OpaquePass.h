#pragma once
#include <memory>
#include "RendererHelpers/PipelineState.h"
#include "RendererHelpers/CBs/ConstantBuffer.h"
#include "RendererHelpers/CBs/CBPerObj.h"
//#include "RendererHelpers/Model.h"


class Mesh;
class Model;

class OpaquePass
{
public:
	OpaquePass(ID3D11Device* pDevice, PipelineState* pipelineState);
	~OpaquePass();

	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	void PreRender();
	void Render(Model& model);
	void PostRender();
	void Destroy();

private:
	ID3D11Device* mDevice;

	bool m_Enabled;
	//ConstantBuffer<cbPerObjectMatrix> m_PerObjectConstantBuffer;
	PipelineState* m_Pipeline;
};