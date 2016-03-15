#pragma once
#include "AppHelpers/Components/RenderableComponent.h"

/*_____START TEMP ZONE_____*/ //To Move to Renderer
#include "AppHelpers/Renderer/RendererHelpers/CBs/ConstantBuffer.h"
#include "AppHelpers/Renderer/RendererHelpers/CBs/CBRefs.h"
/*_____END TEMP ZONE_____*/


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

	//Auxillary
	void UpdateUserInput();
	
	//ATTN: ANIMATION IS BEING HANDLED BY THIS CLASS
	//AND NOT THE RENDERER DUE TO TIME CONSTRAINTS
	//THESE FUNCTIONS WILL BE REWORKED SHORTLY.
	/*_____START TEMP ZONE_____*/ //To Move to Renderer
	void AnimationOneShot();
	void UpdateAnimationConstantBuffers();
	/*_____END TEMP ZONE_____*/


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
	/*_____START TEMP ZONE_____*/
	XMFLOAT4X4 mAnimatedBoxWorldMatrix;
	vector<ID3D11Buffer*> mVertexBuffers;
	vector<ID3D11Buffer*> mIndexBuffers;
	vector<UINT> mIndexCounts;
	vector<ID3D11ShaderResourceView*> mColorTextures;

	Model* mSkinnedModel;
	SkinnedMaterial* mMaterial;
	AnimationPlayer* mAnimationPlayer;
	ID3D11InputLayout* mAnimationInputLayout;
	ID3D11VertexShader* mAnimationVS;
	ID3D11PixelShader* mAnimationPS;
	ConstantBuffer<cbPerObjectMatrixTemp> mModelMatrixDataConstBuffer;
	ConstantBuffer<cbPerObjectSkinningData> mModelSkinningDataConstBuffer;
	ConstantBuffer<cbPerFrameLightCameraData> mScenePerFrameCameraConstBuffer;
	ID3D11SamplerState* mAnimationSampler;
	ID3D11RasterizerState* mNoCullRasterState;
	ID3D11DepthStencilState* mLessEqualDSS;
	ID3D11BlendState* mOpaqueBlendState;
	/*_____END TEMP ZONE_____*/
};