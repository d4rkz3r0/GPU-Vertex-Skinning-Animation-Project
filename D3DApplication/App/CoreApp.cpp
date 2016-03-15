#include "CoreApp.h"
#include "BaseApp.h"
#include "Shlwapi.h"
#include "../Shared/SharedUtils.h"
#include "AppHelpers/Input/Mouse.h"
#include "AppHelpers/Input/Keyboard.h"
#include "AppHelpers/Misc/Exception.h"
#include "AppHelpers/Renderer/Renderer.h"
#include "AppHelpers/Cameras/BaseCamera.h"
#include "AppHelpers/Renderer/RendererHelpers/Model.h"
#include "AppHelpers/Renderer/RendererHelpers/Mesh.h"
#include "AppHelpers/Renderer/RendererHelpers/Animation/AnimationPlayer.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <sstream>
#include <iomanip>

/*_____START TEMP ZONE_____*/
#include "AppHelpers/Renderer/RendererHelpers/SkinnedMaterial.h"
#include <WICTextureLoader.h>
//Shaders
#include "AppHelpers/Renderer/Shaders/Compiled/SkinnedAnimVS.csh"
#include "AppHelpers/Renderer/Shaders/Compiled/SkinnedAnimPS.csh"
#include "AppHelpers/Renderer/RendererHelpers/ModelMaterial.h"

//Buffers
#include "AppHelpers/Renderer/RendererHelpers/CBs/ConstantBuffer.h"
#include "AppHelpers/Renderer/RendererHelpers/CBs/CBRefs.h"
/*_____END TEMP ZONE_____*/


RTTI_DEFINITIONS(CoreApp)

CoreApp::CoreApp(BaseApp& baseApp, BaseCamera& camera) : RenderableComponent(baseApp, camera), mMouse(nullptr), mKeyboard(nullptr), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(XMFLOAT2(5.0f, 5.0f))
{

}

CoreApp::~CoreApp()
{
	for (ID3D11Buffer* vertexBuffer : mVertexBuffers)
	{
		ReleaseObject(vertexBuffer);
	}

	for (ID3D11Buffer* indexBuffer : mIndexBuffers)
	{
		ReleaseObject(indexBuffer);
	}

	for (ID3D11ShaderResourceView* colorTexture : mColorTextures)
	{
		ReleaseObject(colorTexture);
	}

	DeleteObject(mSkinnedModel);
	DeleteObject(mAnimationPlayer);
	DeleteObject(mMaterial);
}

void CoreApp::Initialize()
{
	//Retrieve Input Services
	mKeyboard = static_cast<Keyboard*>(mBaseApp->GetServices().GetService(Keyboard::TypeIdClass()));
	assert(mKeyboard != nullptr);
	mMouse = static_cast<Mouse*>(mBaseApp->GetServices().GetService(Mouse::TypeIdClass()));
	assert(mMouse != nullptr);

	//Load Font
	mSpriteBatch = make_unique<SpriteBatch>(mBaseApp->D3DDevCon());
	mSpriteFont = make_unique<SpriteFont>(mBaseApp->D3DDevice(), L"Assets\\Fonts\\appFont.spritefont");

	//Load Models
	AnimationOneShot();


	//Initialize Shared SceneNode Matrices
	mCurrWorldMX = XMMatrixIdentity();
	mWorldViewProjMX = XMMatrixIdentity();

	//Start Renderer
	mRenderer = new Renderer(mBaseApp->D3DDevice(), mBaseApp->BackBufferRTV(), mBaseApp->BackBufferDSV());
}

void CoreApp::Update(const Time& deltaTime)
{
	//Update Object's Transformations and Materials
	//mRenderer->Update();

	UpdateUserInput();
	mAnimationPlayer->Update(deltaTime);
}

void CoreApp::Draw(const Time& deltaTime)
{
	float mClearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	ID3D11DeviceContext* direct3DDeviceContext = mBaseApp->D3DDevCon();
	direct3DDeviceContext->ClearRenderTargetView(mBaseApp->BackBufferRTV(), reinterpret_cast<const float*>(&mClearColor));
	direct3DDeviceContext->ClearDepthStencilView(mBaseApp->BackBufferDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	direct3DDeviceContext->IASetInputLayout(mAnimationInputLayout);

	UINT stride = mMaterial->VertexSize();
	UINT offset = 0;

	for (UINT i = 0; i < mVertexBuffers.size(); i++)
	{
		ID3D11Buffer* vertexBuffer = mVertexBuffers[i];
		ID3D11Buffer* indexBuffer = mIndexBuffers[i];
		UINT indexCount = mIndexCounts[i];
		ID3D11ShaderResourceView* colorTexture = mColorTextures[i];

		direct3DDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		direct3DDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		direct3DDeviceContext->VSSetShader(mAnimationVS, nullptr, 0);
		direct3DDeviceContext->PSSetShader(mAnimationPS, nullptr, 0);
		UpdateAnimationConstantBuffers();
		direct3DDeviceContext->PSSetSamplers(2, 1, &mAnimationSampler);
		direct3DDeviceContext->PSSetShaderResources(8, 1, &colorTexture);
		direct3DDeviceContext->RSSetState(mNoCullRasterState);

		direct3DDeviceContext->DrawIndexed(indexCount, 0, 0);
	}

	mSpriteBatch->Begin();
	wostringstream textLabel;
	textLabel << "W/A/S/D->Move\nQ/E->Fly\nB->Bind Pose\nR->Restart Anim\nSpace->Pause\nEsc->Quit";
	mSpriteFont->DrawString(mSpriteBatch.get(), textLabel.str().c_str(), mTextPosition);
	mSpriteBatch->End();
}

void CoreApp::UpdateUserInput()
{
	if(mKeyboard)
	{
		//Stop Advancing KeyFrames
 		if (mKeyboard->WasKeyPressedThisFrame(DIK_SPACE))
		{
			if (mAnimationPlayer->IsPlayingClip())
			{
				mAnimationPlayer->PauseClip();
			}
			else
			{
				mAnimationPlayer->ResumeClip();
			}
		}
		//Reset And Start Playing From Bind Pose
		if (mKeyboard->WasKeyPressedThisFrame(DIK_R))
		{
			mAnimationPlayer->StartClip(*(mSkinnedModel->Animations().at(0)));
		}
		//Pause At Bind Pose
		if (mKeyboard->WasKeyPressedThisFrame(DIK_B))
		{
			mAnimationPlayer->StartClip(*(mSkinnedModel->Animations().at(0)));
			mAnimationPlayer->PauseClip();
		}
	}
}

/*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*/
/*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*/
/*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*/
/*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*/
/*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*/
/*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*//*_____ENTERING TEMP ZONE_____*/
void CoreApp::AnimationOneShot()
{
	//Vertex Info
	D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	mBaseApp->D3DDevice()->CreateInputLayout(inputElementDescriptions, ARRAYSIZE(inputElementDescriptions), SkinnedAnimVS, sizeof(SkinnedAnimVS), &mAnimationInputLayout);

	//Shader Creation
	mBaseApp->D3DDevice()->CreateVertexShader(SkinnedAnimVS, sizeof(SkinnedAnimVS), nullptr, &mAnimationVS);
	mBaseApp->D3DDevice()->CreatePixelShader(SkinnedAnimPS, sizeof(SkinnedAnimPS), nullptr, &mAnimationPS);
	
	//State Objects Creation
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	mBaseApp->D3DDevice()->CreateSamplerState(&sd, &mAnimationSampler);

	D3D11_RASTERIZER_DESC rs1;
	ZeroMemory(&rs1, sizeof(rs1));
	rs1.FillMode = D3D11_FILL_SOLID;
	rs1.CullMode = D3D11_CULL_BACK;
	rs1.MultisampleEnable = true;
	mNoCullRasterState = NULL;
	mBaseApp->D3DDevice()->CreateRasterizerState(&rs1, &mNoCullRasterState);

	D3D11_DEPTH_STENCIL_DESC dsd;
	ZeroMemory(&dsd, sizeof(dsd));
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	mLessEqualDSS = NULL;
	mBaseApp->D3DDevice()->CreateDepthStencilState(&dsd, &mLessEqualDSS);

	//Model Loading
	mSkinnedModel = new Model(*mBaseApp, "Assets\\Geometry\\FBX\\Box_Jump.fbx", true);

	//Material Creation
	mMaterial = new SkinnedMaterial();

	//Buffer Resizing <- Not How The Renderer Handles It
	mVertexBuffers.resize(mSkinnedModel->Meshes().size());
	mIndexBuffers.resize(mSkinnedModel->Meshes().size());
	mIndexCounts.resize(mSkinnedModel->Meshes().size());
	mColorTextures.resize(mSkinnedModel->Meshes().size());

	//Constant Buffer Creation
	mModelMatrixDataConstBuffer.Initialize(mBaseApp->D3DDevice());
	mModelSkinningDataConstBuffer.Initialize(mBaseApp->D3DDevice());
	mScenePerFrameCameraConstBuffer.Initialize(mBaseApp->D3DDevice());

	//Mesh, Material, Buffer Loading
	for (UINT i = 0; i < mSkinnedModel->Meshes().size(); i++)
	{
		Mesh* mesh = mSkinnedModel->Meshes().at(i);

		ID3D11Buffer* vertexBuffer = nullptr;
		mMaterial->CreateVertexBuffer(mBaseApp->D3DDevice(), *mesh, &vertexBuffer);
		mVertexBuffers[i] = vertexBuffer;

		ID3D11Buffer* indexBuffer = nullptr;
		mesh->CreateIndexBuffer(&indexBuffer);
		mIndexBuffers[i] = indexBuffer;

		mIndexCounts[i] = mesh->Indices().size();

		ID3D11ShaderResourceView* colorTexture = nullptr;
		ModelMaterial* material = mesh->GetMaterial();

		if (material != nullptr && material->Textures().find(TextureTypeDiffuse) != material->Textures().cend())
		{
			vector<wstring>* diffuseTextures = material->Textures().at(TextureTypeDiffuse);
			wstring filename = PathFindFileName(diffuseTextures->at(0).c_str());

			wostringstream textureName;
			textureName << L"Assets\\Textures\\ColorMaps\\" << filename.substr(0, filename.length() - 4) << L".jpg";
			HRESULT hr = CreateWICTextureFromFile(mBaseApp->D3DDevice(), mBaseApp->D3DDevCon(), textureName.str().c_str(), nullptr, &colorTexture);
			if (FAILED(hr))
			{
				throw Exception("Can't Load The Color Map Brah.", hr);
			}
		}
		mColorTextures[i] = colorTexture;
	}


	//Model Orientation
	XMStoreFloat4x4(&mAnimatedBoxWorldMatrix, XMMatrixIdentity());

	//Create Player and Start Animating
	mAnimationPlayer = new AnimationPlayer(*mBaseApp, *mSkinnedModel);
	mAnimationPlayer->StartClip(*(mSkinnedModel->Animations().at(0)));
}

void CoreApp::UpdateAnimationConstantBuffers()
{
	//SLOT 8//
	cbPerFrameLightCameraData scenePerFrameCamInfo;
	scenePerFrameCamInfo.AmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	scenePerFrameCamInfo.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	scenePerFrameCamInfo.LightPosition = XMFLOAT3(0.0f, 4.0f, 4.0f);
	scenePerFrameCamInfo.LightRadius = 30.0f;
	XMStoreFloat3(&scenePerFrameCamInfo.CameraPosition, mCamera->GetPositionXM());
	mScenePerFrameCameraConstBuffer.Data = scenePerFrameCamInfo;
	mScenePerFrameCameraConstBuffer.ApplyChanges(mBaseApp->D3DDevCon());
	auto cBufferPerFrame = mScenePerFrameCameraConstBuffer.Buffer();
	mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);
	mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);


	//SLOT 9//
	cbPerObjectMatrixTemp modelMatrixTempInfo;
	modelMatrixTempInfo.SpecularColor =  XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	modelMatrixTempInfo.SpecularPower = 25.0f;
	XMMATRIX worldMatrix = XMLoadFloat4x4(&mAnimatedBoxWorldMatrix);
	XMStoreFloat4x4(&modelMatrixTempInfo.World, worldMatrix);
	XMMATRIX wvp = worldMatrix * mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
	XMStoreFloat4x4(&modelMatrixTempInfo.WorldViewProjection, wvp);
	mModelMatrixDataConstBuffer.Data = modelMatrixTempInfo;
	mModelMatrixDataConstBuffer.ApplyChanges(mBaseApp->D3DDevCon());
	auto cBufferPerObjMatrix = mModelMatrixDataConstBuffer.Buffer();
	mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_OBJECT_MATRIX_TEMP_REGISTER_SLOT, 1, &cBufferPerObjMatrix);
	mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_OBJECT_MATRIX_TEMP_REGISTER_SLOT, 1, &cBufferPerObjMatrix);


	//SLOT 10//
	cbPerObjectSkinningData modelSkinningData;
	copy(mAnimationPlayer->BoneTransforms().begin(), mAnimationPlayer->BoneTransforms().end(), modelSkinningData.BoneTransforms);
	mModelSkinningDataConstBuffer.Data = modelSkinningData;
	mModelSkinningDataConstBuffer.ApplyChanges(mBaseApp->D3DDevCon());
	auto cBufferPerObjBoneMatrices = mModelSkinningDataConstBuffer.Buffer();
	mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &cBufferPerObjBoneMatrices);
	mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &cBufferPerObjBoneMatrices);
}
/*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*/
/*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*/
/*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*/
/*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*/
/*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*/
/*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*//*_____END TEMP ZONE_____*/