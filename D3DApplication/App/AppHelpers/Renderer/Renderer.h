#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
using namespace std;
using namespace DirectX;

//Forward Declarations
class DepthStencilStateManager;
class RasterizerStateManager;
class SamplerStateManager;
class BlendStateManager;
class StructuredBuffer;
class TransparentPass;
class PipelineState;
class AnimatedMesh;
class IndexBuffer;
class Camera;
class OpaquePass;
class Technique;
class ClearPass;
class Shader;

class Renderer
{
public:
	Renderer(ID3D11Device* theDevice, Camera& camera, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* backBufferDSV);
	~Renderer();

	//Core
	void Initialize();
	void Draw(float deltaTime);
	void Update();
	void Destroy();

	//Pipeline
	void SaveStates();
	void RestoreStates();

private:
	//-Shared D3D Objects-//
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;

	//-Shared Custom Managers-//
	DepthStencilStateManager* mDepthStencilStateManager;
	RasterizerStateManager* mRasterizerStateManager;
	SamplerStateManager* mSamplerStateManager;
	BlendStateManager* mBlendStateManager;

	//-Rendering Techniques-//
	Technique* mSkinnedAnimationForwardRenderingTechnique;

	//-Rendering Passes-//
	ClearPass* mClearPass;
	OpaquePass* mOpaquePass;
	TransparentPass* mTransparentPass;

	//-Pipeline States-//
	PipelineState* mOpaquePipeLineState;
	PipelineState* mTransparentPipeLineState;

	//-Shaders-//
	Shader* mAnimationVertexShader;
	Shader* mAnimationPixelShader;

	//-Game Objects-//
	vector<AnimatedMesh*> mOpaqueGeometry;
	vector<AnimatedMesh*> mTransparentGeometry;

	//Misc
	Camera* mCamera;
	float mClearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
};