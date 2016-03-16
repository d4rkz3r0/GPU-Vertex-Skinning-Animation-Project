#pragma once
#include "AppHelpers/Components/RenderableComponent.h"

//To Move to Renderer
#include "AppHelpers/Renderer/RendererHelpers/CBs/ConstantBuffer.h"
#include "AppHelpers/Renderer/RendererHelpers/CBs/CBRefs.h"
#include <sys/stat.h>


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



	bool loadingFromFBX = true;
	bool loadingFromBIN = false;
	vector<string> mFBXFileNames;
	vector<string> mBINFileNames;
	vector<bool> mBinaryTrackers;
	//void LoadModelFromBinary(const string& filePath);
	//void WriteOutToBinary();

	bool fileExists(const string& filename)
	{

		struct stat buf;
		if (stat(filename.c_str(), &buf) != -1)
		{
			return true;
		}
		return false;
	}

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
	ID3D11BlendState* mTransparentBlendState;

	//Depth
	ID3D11DepthStencilState* mDefaultDSS;
	ID3D11DepthStencilState* mLessEqualDSS;
	ID3D11DepthStencilState* mNoDepthDSS;

	//Sampler States
	ID3D11SamplerState* mAnisoSamplerState;

	ID3D11InputLayout* mAnimationInputLayout;
	ID3D11VertexShader* mAnimationVS;
	ID3D11PixelShader* mAnimationPS;
	float mClearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	float mBlendFactor[4] = { 0.75f, 0.75f, 0.75f, 1.0f };

	float mtBox1Distance;
	float mtBox2Distance;

	ID3D11RasterizerState* CCWcullMode_;
	ID3D11RasterizerState* CWcullMode_;
	/*_____END TEMP ZONE_____*/// ->To Move to Renderer
};