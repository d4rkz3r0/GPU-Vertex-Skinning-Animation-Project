#include "PipelineState.h"

PipelineState::PipelineState(ID3D11Device* pDevice) : mDevice(pDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);
	mBlendFactor = NULL;
	mSampleMask = 0xFFFFFFFF;
	mStencilRef = 0;

	mBlendState = nullptr;
	mRasterizerState = nullptr;
	mDepthStencilState = nullptr;
}

PipelineState::~PipelineState()
{
	mDevice->Release();
	mDeviceContext->Release();
	mBlendState->Release();
	mRasterizerState->Release();
	mDepthStencilState->Release();
	mShaders.clear();
	delete mBlendFactor;
}

void PipelineState::SetShader(Shader::ShaderType type, Shader* pShader)
{
	mShaders[type] = pShader;
}

Shader* PipelineState::GetShader(Shader::ShaderType type)
{
	ShaderMap::const_iterator iter = mShaders.find(type);
	if (iter != mShaders.end())
	{
		return iter->second;
	}

	return nullptr;
}

ShaderMap& PipelineState::GetShaders()
{
	return mShaders;
}

void PipelineState::SetBlendState(ID3D11BlendState* blendState, FLOAT* blendFactor, UINT sampleMask)
{
	mBlendState = blendState;
	mBlendFactor = blendFactor;
	mSampleMask = sampleMask;
}

ID3D11BlendState& PipelineState::GetBlendState()
{
	return *mBlendState;
}

void PipelineState::SetRasterizerState(ID3D11RasterizerState* rasterizerState)
{
	mRasterizerState = rasterizerState;
}

ID3D11RasterizerState& PipelineState::GetRasterizerState()
{
	return *mRasterizerState;
}

void PipelineState::SetDepthStencilState(ID3D11DepthStencilState* depthStencilState, UINT stencilRef)
{
	mDepthStencilState = depthStencilState;
	mStencilRef = stencilRef;
}

ID3D11DepthStencilState& PipelineState::GetDepthStencilState()
{
	return *mDepthStencilState;
}

void PipelineState::BindStates()
{
	mDeviceContext->RSSetState(mRasterizerState);
	mDeviceContext->OMSetBlendState(mBlendState, mBlendFactor, mSampleMask);
	mDeviceContext->OMSetDepthStencilState(mDepthStencilState, mStencilRef);

	for (auto shader : mShaders)
	{
		Shader* pShader = shader.second;
		if (pShader != nullptr)
		{
			pShader->Bind();
		}
	}
}

void PipelineState::UnBindStates()
{

	for (auto shader : mShaders)
	{
		Shader* pShader = shader.second;
		if (pShader)
		{
			pShader->UnBind();
		}
	}
}