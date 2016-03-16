#pragma once
#include "AppHelpers/Components/RenderableComponent.h"

//To Move to Renderer
#include "AppHelpers/Renderer/RendererHelpers/CBs/ConstantBuffer.h"
#include "AppHelpers/Renderer/RendererHelpers/CBs/CBRefs.h"


//Forward Declarations
class Model;
class AnimationPlayer;
class SkinnedMaterial;
class BaseCamera;
class Renderer;
class Keyboard;
class Mouse;

namespace DirectX
{
	class SpriteBatch;
	class SpriteFont;
}

class CoreApp : public RenderableComponent
{
	RTTI_DECLARATIONS(CoreApp, RenderableComponent)

public:
	CoreApp(BaseApp& baseApp, BaseCamera& camera);
	~CoreApp();

	//Core Overrides
	virtual void Initialize() override;
	virtual void Update(const Time& deltaTime) override;
	virtual void Draw(const Time& deltaTime) override;

	//Auxiliary
	void UpdateUserInput();

	//To Move to Renderer
	void AnimationOneShot();
	void BuildStuff();
	void ConstantBufferUpdate(UINT index);

private:
	CoreApp() = delete;
	CoreApp(const CoreApp& rhs) = delete;
	CoreApp& operator=(const CoreApp& rhs) = delete;

	//DirectInput
	Mouse* mMouse;
	Keyboard* mKeyboard;

	//DirectXTK Font Rendering
	unique_ptr<SpriteBatch> mSpriteBatch;
	unique_ptr<SpriteFont> mSpriteFont;
	XMFLOAT2 mTextPosition;

	//Renderer
	Renderer* mRenderer;

	//-SceneObjects-//
	//Shared Matrices
	XMMATRIX mCurrWorldMX;
	XMMATRIX mWorldViewProjMX;
	XMMATRIX mViewProjMX;

	//GameObjects
	/*_____START TEMP ZONE_____*/// -> To Move to Renderer
	vector<XMFLOAT4X4> mModelsWorldMatrices;
	vector<ID3D11Buffer*> mVertexBuffers;
	vector<ID3D11Buffer*> mIndexBuffers;
	vector<UINT> mIndexCounts;
	vector<ID3D11ShaderResourceView*> mColorTextures;

	vector<Model*> mSkinnedModels;
	vector<SkinnedMaterial*> mSkinnedMaterials;
	vector<AnimationPlayer*> mAnimationPlayers;
	UINT mTotalNumMeshes = 0;

	vector<ConstantBuffer<cbPerObjectMatrixTemp>*> mModelMatrixConstantBuffers;
	vector<ConstantBuffer<cbPerObjectSkinningData>*> mModelBoneTransformsConstantBuffers;
	ConstantBuffer<cbPerFrameLightCameraData> mScenePerFrameCameraConstBuffer;

	//Raster
	ID3D11RasterizerState* mDefaultRasterState;
	ID3D11RasterizerState* mNoCullRasterState;
	ID3D11RasterizerState* mCCWCullRasterState;
	ID3D11RasterizerState* mWireFrameRasterState;

	//Blend
	ID3D11BlendState* mDefaultBlendState;
	ID3D11BlendState* mAlphaCoverageBlendState;
	ID3D11BlendState* mAdditiveBlendState;

	//Depth
	ID3D11DepthStencilState* mDefaultDSS;
	ID3D11DepthStencilState* mLessEqualDSS;
	ID3D11DepthStencilState* mNoDepthDSS;

	//Sampler States
	ID3D11SamplerState* mAnisoSamplerState;

	ID3D11InputLayout* mAnimationInputLayout;
	ID3D11VertexShader* mAnimationVS;
	ID3D11PixelShader* mAnimationPS;

	
	/*_____END TEMP ZONE_____*/// ->To Move to Renderer
};