#pragma once
#include <d3d11.h>
#include <memory>
#include <map>
#include "RendererHelpers/Shader.h"
#include "RendererHelpers/BlendStateManager.h"
#include "RendererHelpers/RasterizerStateManager.h"
#include "RendererHelpers/DepthStencilStateManager.h"

typedef std::map<Shader::ShaderType, Shader*> ShaderMap;

//ToDo Parameters need reworking for mutators

class PipelineState
{
	friend class RasterizerStateManager;
	friend class BlendStateManager;
	friend class DepthStencilStateManager;

public:
	PipelineState(ID3D11Device* pDevice);
	~PipelineState();

	void SetShader(Shader::ShaderType type, Shader* pShader);
	Shader* GetShader(Shader::ShaderType type);
	ShaderMap& GetShaders();

	void SetBlendState(ID3D11BlendState* blendState, FLOAT* blendFactor, UINT sampleMask);
	ID3D11BlendState& GetBlendState();

	void SetRasterizerState(ID3D11RasterizerState* rasterizerState);
	ID3D11RasterizerState& GetRasterizerState();

	void SetDepthStencilState(ID3D11DepthStencilState* depthStencilState, UINT stencilRef);
	ID3D11DepthStencilState& GetDepthStencilState();

	//void BindStates(RasterizerStateManager::RasterStateType rasterState, BlendStateManager::BlendStateType blendState ,DepthStencilStateManager::DepthStencilStateType depthState);
	void BindStates();
	void UnBindStates();

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;

	ID3D11BlendState* mBlendState;
	FLOAT* mBlendFactor;
	UINT mSampleMask;

	ID3D11RasterizerState* mRasterizerState;
	ID3D11DepthStencilState* mDepthStencilState;
	UINT mStencilRef;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


	ShaderMap mShaders;
};
