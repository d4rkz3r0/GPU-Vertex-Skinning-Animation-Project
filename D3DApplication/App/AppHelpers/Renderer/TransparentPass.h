#pragma once
#include <memory>
#include "RendererHelpers/PipelineState.h"
#include "RendererHelpers/CBs/ConstantBuffer.h"
#include "RendererHelpers/CBs/CBPerObj.h"
//#include "RendererHelpers/Model.h"


class Mesh;
class Model;

class TransparentPass
{
public:
	TransparentPass(ID3D11Device* pDevice, PipelineState* pipelineState);
	~TransparentPass();

	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	void PreRender();
	void Render(Model& model);
	void PostRender();
	void Destroy();

private:
	ID3D11Device* mDevice;

	bool m_Enabled;
	PipelineState* m_Pipeline;
};