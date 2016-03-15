#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "MeshStructs.h"
#include "RendererHelpers/CBs/ConstantBuffer.h"
#include "RendererHelpers/CBs/CBRefs.h"
#include "RendererHelpers/CBs/Light.h"
#include "RendererHelpers/StructuredBuffer.h"
#include "TransparentPass.h"
#include "OpaquePass.h"
#include "ClearPass.h"
#include "Technique.h"
using namespace DirectX;

//Forward Declarations
class DepthStencilStateManager;
class RasterizerStateManager;
class VertexBufferManager;
class SamplerStateManager;
class BlendStateManager;
class StructuredBuffer;
class PipelineState;
class IndexBuffer;
class Shader;

class Renderer
{
public:
	Renderer(ID3D11Device* theDevice, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV);
	~Renderer();
	
	//Core
	void Initialize();
	void PreRender();
	void Render();
	void PostRender();
	void Update();
	void Destroy();

	//Lighting
	void BuildLights(UINT numLightsToBuild);
	void UpdateLights();
	void UpdateNumLights();

	//Pipeline
	void SaveStates();
	void RestoreStates();

	//Constant Buffers
	void PopulateMaterialConstantBuffer();
	void SetPerObjectData(XMFLOAT4X4 &mMVP, XMFLOAT4X4 &mWorld);
	void SetPerObjectData(XMMATRIX &mMVP, XMMATRIX &mWorld);

	//Structured Buffer
	/*template<typename T>
	void CreateStructuredBuffer(const vector<T>& data, CPUAccess cpuAccess, bool gpuWrite);*/

private:
	//-Shared D3D Objects-//
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;

	//-Shared Custom Managers-//
	BlendStateManager* mBlendStateManager;
	DepthStencilStateManager* mDepthStencilStateManager;
	RasterizerStateManager* mRasterizerStateManager;
	SamplerStateManager* mSamplerStateManager;
	VertexBufferManager* mVertexBufferManager;

	//-Forward Rendering-//
	Technique* mForwardRenderingTechnique;

	//Shaders Required
	Shader* g_pVertexShader;
	Shader* g_pPixelShader;
	Shader* g_pLightPixelShader;

	//Pipeline Configurations Required
	PipelineState* g_pOpaquePipeline;
	PipelineState* g_pTransparentPipeline;

	//Rendering Passes
	OpaquePass* g_OpaquePass;
	TransparentPass* g_TransparentPass;
	ClearPass* g_ClearPass;

	//Constant Buffers Required
	ConstantBuffer<cbPerObjectMatrix> mPerObjectMatrixData;
	ConstantBuffer<cbPerObjectMaterial> mPerObjectMaterialData;

	//Lights
	vector<Light*> mLights;
	Light* g_pCurrentLight;
	uint32_t g_uiCurrentLightIndex = 0;
	StructuredBuffer* g_pLightsStructuredBuffer;
	UINT mNumLightsToMake;


	//Misc
	float mBlackClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float mWhiteClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
};