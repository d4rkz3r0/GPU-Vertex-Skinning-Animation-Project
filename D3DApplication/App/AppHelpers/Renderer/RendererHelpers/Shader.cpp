#include "Shader.h"
#include "ShaderRefs.h"
#include "../../../../Shared/SharedUtils.h"


Shader::Shader(ID3D11Device* pDevice) : m_ShaderType(UnknownShaderType), mDevice(pDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);
	mVertexShader = nullptr;
	mPixelShader = nullptr;
	mHullShader = nullptr;
	mDomainShader = nullptr;
	mGeometryShader = nullptr;
	mComputeShader = nullptr;
	mInputLayout = nullptr;
	mSamplerState = nullptr;
}

Shader::~Shader()
{
	Destroy();
}

Shader::ShaderType Shader::GetType()
{
	return m_ShaderType;
}

void Shader::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);

	ReleaseObject(mVertexShader);
	ReleaseObject(mHullShader);
	ReleaseObject(mDomainShader);
	ReleaseObject(mGeometryShader);
	ReleaseObject(mPixelShader);
	ReleaseObject(mComputeShader);
	ReleaseObject(mInputLayout);
	ReleaseObject(mSamplerState);
}

void Shader::LoadShader(ShaderType type, UINT PSType)
{
	m_ShaderType = type;

	switch (m_ShaderType)
	{
		case VertexShader:
		{
			mDevice->CreateVertexShader(ForwardRenderingVSMain, sizeof(ForwardRenderingVSMain), nullptr, &mVertexShader);
			D3D11_INPUT_ELEMENT_DESC inputElementDescription[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			mDevice->CreateInputLayout(inputElementDescription, ARRAYSIZE(inputElementDescription), ForwardRenderingVSMain, sizeof(ForwardRenderingVSMain), &mInputLayout);
			break;
		}

		case PixelShader:
		{
			switch (PSType)
			{
				case 0:
				{
					mDevice->CreatePixelShader(ForwardRenderingPSMain, sizeof(ForwardRenderingPSMain), nullptr, &mPixelShader);
					break;
				}
				case 1:
				{
					mDevice->CreatePixelShader(ForwardRenderingPSLight, sizeof(ForwardRenderingPSLight), nullptr, &mPixelShader);
					break;
				}
			}
		}
		
		default:
		{
			//Unsupported Shader Type
			break;
		}
	}
}


void Shader::Bind()
{
	if (mVertexShader != nullptr)
	{
		mDeviceContext->IASetInputLayout(mInputLayout);
		mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	}
	else if (mPixelShader != nullptr)
	{
		mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
	}
}

void Shader::UnBind()
{
	if (mVertexShader != nullptr)
	{
		mDeviceContext->IASetInputLayout(nullptr);
		mDeviceContext->VSSetShader(nullptr, nullptr, 0);
	}
	else if (mPixelShader != nullptr)
	{
		mDeviceContext->PSSetShader(nullptr, nullptr, 0);
	}
}

ID3D11SamplerState* Shader::GetSamplerState()
{
	return mSamplerState;
}

void Shader::SetSamplerState(ID3D11SamplerState* samplerState)
{
	mSamplerState = samplerState;
}
