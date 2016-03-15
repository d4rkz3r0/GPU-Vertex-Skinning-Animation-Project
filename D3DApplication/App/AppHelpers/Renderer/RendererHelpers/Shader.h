#pragma once
#include <map>
#include <d3d11.h>
#include <memory>

class Shader
{
public:
	enum ShaderType
	{
		UnknownShaderType = 0,
		VertexShader,
		HullShader,
		DomainShader,
		GeometryShader,
		PixelShader,
		ComputeShader
	};


	Shader(ID3D11Device* pDevice);
	virtual ~Shader();

	ShaderType GetType();
	void Destroy();

	void LoadShader(ShaderType type, UINT PSType = -1);

	void Bind();
	void UnBind();
	ID3D11SamplerState* GetSamplerState();
	void SetSamplerState(ID3D11SamplerState* samplerState);

	ShaderType	m_ShaderType;
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11VertexShader* mVertexShader;
	ID3D11HullShader* mHullShader;
	ID3D11DomainShader* mDomainShader;
	ID3D11GeometryShader* mGeometryShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11ComputeShader* mComputeShader;
	ID3D11InputLayout* mInputLayout;
	ID3D11SamplerState* mSamplerState;
};

