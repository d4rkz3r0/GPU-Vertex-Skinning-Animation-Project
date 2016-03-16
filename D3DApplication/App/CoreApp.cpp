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
#include <fstream>
#include <iostream>


/*_____START TEMP ZONE_____*/// -> To Move to Renderer
#include "AppHelpers/Renderer/RendererHelpers/SkinnedMaterial.h"
#include <WICTextureLoader.h>
//Shaders
#include "AppHelpers/Renderer/Shaders/Compiled/SkinnedAnimVS.csh"
#include "AppHelpers/Renderer/Shaders/Compiled/SkinnedAnimPS.csh"
#include "AppHelpers/Renderer/RendererHelpers/ModelMaterial.h"
//Buffers
#include "AppHelpers/Renderer/RendererHelpers/CBs/ConstantBuffer.h"
#include "AppHelpers/Renderer/RendererHelpers/CBs/CBRefs.h"
#include <algorithm>
/*_____END TEMP ZONE_____*/// -> To Move to Renderer


RTTI_DEFINITIONS(CoreApp)

CoreApp::CoreApp(BaseApp& baseApp, BaseCamera& camera) : RenderableComponent(baseApp, camera), mMouse(nullptr), mKeyboard(nullptr), mSpriteBatch(nullptr), mSpriteFont(nullptr), mTextPosition(XMFLOAT2(5.0f, 5.0f))
{ }

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

	mSkinnedModels = vector<Model*>();
	mSkinnedMaterials = vector<SkinnedMaterial*>();
	mAnimationPlayers = vector<AnimationPlayer*>();
	mModelMatrixConstantBuffers = vector<ConstantBuffer<cbPerObjectMatrixTemp>*>();
	mModelBoneTransformsConstantBuffers = vector<ConstantBuffer<cbPerObjectSkinningData>*>();

	ReleaseObject(mWireFrameRasterState);
	ReleaseObject(mNoCullRasterState);
	ReleaseObject(mDefaultRasterState);
	ReleaseObject(mAnisoSamplerState);
	ReleaseObject(mLessEqualDSS);
	ReleaseObject(mCCWCullRasterState);
	ReleaseObject(mAnimationInputLayout);
	ReleaseObject(mAnimationVS);
	ReleaseObject(mAnimationPS);
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

	//PipeLine Stuff
	BuildStuff();


	mBINFileNames.resize(6);
	mBinaryTrackers.resize(6);
	mBINFileNames.at(0) = "Assets\\Geometry\\FBX\\Box_Idle.bin";
	mBINFileNames.at(1) = "Assets\\Geometry\\FBX\\Box_Attack.bin";
	mBINFileNames.at(2) = "Assets\\Geometry\\FBX\\Box_Jump.bin";
	mBINFileNames.at(3) = "Assets\\Geometry\\FBX\\Box_Walk.bin";
	mBINFileNames.at(4) = "Assets\\Geometry\\FBX\\Box_Attack.bin";
	mBINFileNames.at(5) = "Assets\\Geometry\\FBX\\Box_Idle.bin";

	//Load Models
	for (size_t i = 0; i < mBINFileNames.size(); i++)
	{
		if (fileExists(mBINFileNames.at(i)))
		{
			mBinaryTrackers[i] = true;
			//LoadModelFromBinary(mBINFileNames.at(i));
		}
		else
		{
			mBinaryTrackers[i] = false;
		}
	}

	if (std::all_of(begin(mBinaryTrackers), end(mBinaryTrackers), [] (bool i)	{ return i; }))
	{
		loadingFromFBX = false;
		loadingFromBIN = true;
	}
	else
	{
		loadingFromFBX = true;
		loadingFromBIN = false;
	}

	AnimationOneShot();

	//Initialize Shared SceneNode Matrices
	mCurrWorldMX = XMMatrixIdentity();
	mWorldViewProjMX = XMMatrixIdentity();

	//Start Renderer
	//mRenderer = new Renderer(mBaseApp->D3DDevice(), mBaseApp->BackBufferRTV(), mBaseApp->BackBufferDSV());
}

void CoreApp::Update(const Time& deltaTime)
{
	UpdateUserInput();
	for (auto aAnimPlayer : mAnimationPlayers)
	{
		aAnimPlayer->Update(deltaTime);
	}
}

void CoreApp::Draw(const Time& deltaTime)
{
	ID3D11DeviceContext* direct3DDeviceContext = mBaseApp->D3DDevCon();
	direct3DDeviceContext->OMSetDepthStencilState(mLessEqualDSS, 0);
	direct3DDeviceContext->ClearRenderTargetView(mBaseApp->BackBufferRTV(), reinterpret_cast<const float*>(&mClearColor));
	direct3DDeviceContext->ClearDepthStencilView(mBaseApp->BackBufferDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	direct3DDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	direct3DDeviceContext->IASetInputLayout(mAnimationInputLayout);
	ID3D11ShaderResourceView* colorTexture = mColorTextures[0];
	direct3DDeviceContext->PSSetSamplers(2, 1, &mAnisoSamplerState);
	direct3DDeviceContext->PSSetShaderResources(8, 1, &colorTexture);

	for (UINT z = 0; z < mSkinnedModels.size(); z++)
	{
		UINT stride = mSkinnedMaterials.at(z)->VertexSize();
		UINT offset = 0;

		for (UINT i = 0; i < mVertexBuffers.size(); i++)
		{
			ID3D11Buffer* vertexBuffer = mVertexBuffers[i];
			ID3D11Buffer* indexBuffer = mIndexBuffers[i];
			UINT indexCount = mIndexCounts[i];

			direct3DDeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
			direct3DDeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
			direct3DDeviceContext->VSSetShader(mAnimationVS, nullptr, 0);
			direct3DDeviceContext->PSSetShader(mAnimationPS, nullptr, 0);

			switch (z)
			{
			case 0:
			case 1:
			case 2:
			case 3:
			{
				direct3DDeviceContext->OMSetBlendState(mDefaultBlendState, NULL, 0xFFFFFFFF);
				direct3DDeviceContext->RSSetState(mNoCullRasterState);
				break;
			}
			case 4:
			{
				direct3DDeviceContext->OMSetBlendState(mTransparentBlendState, mBlendFactor, 0xFFFFFFFF);
				//Order Transparent Objects
				//Render the objects in depth order to the render target
				XMVECTOR tBoxPosition1 = XMVectorZero();
				tBoxPosition1 = XMVector3TransformCoord(tBoxPosition1, XMLoadFloat4x4(&mModelsWorldMatrices.at(4)));
				float distX = XMVectorGetX(tBoxPosition1) - XMVectorGetX(mCamera->GetPositionXM());
				float distY = XMVectorGetY(tBoxPosition1) - XMVectorGetY(mCamera->GetPositionXM());
				float distZ = XMVectorGetZ(tBoxPosition1) - XMVectorGetZ(mCamera->GetPositionXM());
				mtBox1Distance = distX * distX + distY * distY + distZ * distZ;

				//SLOT 8//
				cbPerFrameLightCameraData scenePerFrameCamInfo;
				scenePerFrameCamInfo.AmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
				scenePerFrameCamInfo.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				scenePerFrameCamInfo.LightPosition = XMFLOAT3(0.0f, 10.0f, 0.0f);
				scenePerFrameCamInfo.LightRadius = 50.0f;
				XMStoreFloat3(&scenePerFrameCamInfo.CameraPosition, mCamera->GetPositionXM());
				mScenePerFrameCameraConstBuffer.Data = scenePerFrameCamInfo;
				mScenePerFrameCameraConstBuffer.ApplyChanges(mBaseApp->D3DDevCon());
				auto cBufferPerFrame = mScenePerFrameCameraConstBuffer.Buffer();
				mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);
				mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);

				//SLOT 9//
				cbPerObjectMatrixTemp modelMatrixTempInfo;
				modelMatrixTempInfo.SpecularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				modelMatrixTempInfo.SpecularPower = 25.0f;
				XMMATRIX worldMatrix = XMLoadFloat4x4(&mModelsWorldMatrices.at(4));
				XMStoreFloat4x4(&modelMatrixTempInfo.World, worldMatrix);
				XMMATRIX wvp = worldMatrix * mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
				XMStoreFloat4x4(&modelMatrixTempInfo.WorldViewProjection, wvp);
				mModelMatrixConstantBuffers.at(4)->Data = modelMatrixTempInfo;
				mModelMatrixConstantBuffers.at(4)->ApplyChanges(mBaseApp->D3DDevCon());
				auto cBufferPerObjMatrix = mModelMatrixConstantBuffers.at(4)->Buffer();
				mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_OBJECT_MATRIX_TEMP_REGISTER_SLOT, 1, &cBufferPerObjMatrix);
				mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_OBJECT_MATRIX_TEMP_REGISTER_SLOT, 1, &cBufferPerObjMatrix);

				//SLOT 10//
				cbPerObjectSkinningData modelSkinningData;
				copy(mAnimationPlayers.at(4)->BoneTransforms().begin(), mAnimationPlayers.at(4)->BoneTransforms().end(), modelSkinningData.BoneTransforms);
				mModelBoneTransformsConstantBuffers.at(4)->Data = modelSkinningData;
				mModelBoneTransformsConstantBuffers.at(4)->ApplyChanges(mBaseApp->D3DDevCon());
				auto cBufferPerObjBoneMatrices = mModelBoneTransformsConstantBuffers.at(4)->Buffer();
				mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &cBufferPerObjBoneMatrices);
				mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &cBufferPerObjBoneMatrices);

				break;
			}
			case 5:
			{
				direct3DDeviceContext->OMSetBlendState(mTransparentBlendState, mBlendFactor, 0xFFFFFFFF);
				//Order Transparent Objects
				//Render the objects in depth order to the render target
				XMVECTOR tBoxPosition1 = XMVectorZero();
				tBoxPosition1 = XMVector3TransformCoord(tBoxPosition1, XMLoadFloat4x4(&mModelsWorldMatrices.at(5)));
				float distX = XMVectorGetX(tBoxPosition1) - XMVectorGetX(mCamera->GetPositionXM());
				float distY = XMVectorGetY(tBoxPosition1) - XMVectorGetY(mCamera->GetPositionXM());
				float distZ = XMVectorGetZ(tBoxPosition1) - XMVectorGetZ(mCamera->GetPositionXM());
				mtBox2Distance = distX * distX + distY * distY + distZ * distZ;

				//SLOT 8//
				cbPerFrameLightCameraData scenePerFrameCamInfo;
				scenePerFrameCamInfo.AmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
				scenePerFrameCamInfo.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				scenePerFrameCamInfo.LightPosition = XMFLOAT3(0.0f, 10.0f, 0.0f);
				scenePerFrameCamInfo.LightRadius = 50.0f;
				XMStoreFloat3(&scenePerFrameCamInfo.CameraPosition, mCamera->GetPositionXM());
				mScenePerFrameCameraConstBuffer.Data = scenePerFrameCamInfo;
				mScenePerFrameCameraConstBuffer.ApplyChanges(mBaseApp->D3DDevCon());
				auto cBufferPerFrame = mScenePerFrameCameraConstBuffer.Buffer();
				mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);
				mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);

				//SLOT 9//
				cbPerObjectMatrixTemp modelMatrixTempInfo;
				modelMatrixTempInfo.SpecularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				modelMatrixTempInfo.SpecularPower = 25.0f;
				XMMATRIX worldMatrix = XMLoadFloat4x4(&mModelsWorldMatrices.at(5));
				XMStoreFloat4x4(&modelMatrixTempInfo.World, worldMatrix);
				XMMATRIX wvp = worldMatrix * mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
				XMStoreFloat4x4(&modelMatrixTempInfo.WorldViewProjection, wvp);
				mModelMatrixConstantBuffers.at(5)->Data = modelMatrixTempInfo;
				mModelMatrixConstantBuffers.at(5)->ApplyChanges(mBaseApp->D3DDevCon());
				auto cBufferPerObjMatrix = mModelMatrixConstantBuffers.at(5)->Buffer();
				mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_OBJECT_MATRIX_TEMP_REGISTER_SLOT, 1, &cBufferPerObjMatrix);
				mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_OBJECT_MATRIX_TEMP_REGISTER_SLOT, 1, &cBufferPerObjMatrix);

				//SLOT 10//
				cbPerObjectSkinningData modelSkinningData;
				copy(mAnimationPlayers.at(5)->BoneTransforms().begin(), mAnimationPlayers.at(5)->BoneTransforms().end(), modelSkinningData.BoneTransforms);
				mModelBoneTransformsConstantBuffers.at(5)->Data = modelSkinningData;
				mModelBoneTransformsConstantBuffers.at(5)->ApplyChanges(mBaseApp->D3DDevCon());
				auto cBufferPerObjBoneMatrices = mModelBoneTransformsConstantBuffers.at(5)->Buffer();
				mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &cBufferPerObjBoneMatrices);
				mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &cBufferPerObjBoneMatrices);

				break;
			}

			default:
			{
				break;
			}
			}

			if (mtBox1Distance < mtBox2Distance)
			{
				//Swap
				XMMATRIX tempMatrix = XMLoadFloat4x4(&mModelsWorldMatrices.at(4));
				mModelsWorldMatrices.at(4) = mModelsWorldMatrices.at(5);
				XMStoreFloat4x4(&mModelsWorldMatrices.at(5), tempMatrix);
			}


			if (z != 4 || z != 5)
			{
				ConstantBufferUpdate(z);
			}
			if (z == 4)
			{
				direct3DDeviceContext->RSSetState(CCWcullMode_);
				direct3DDeviceContext->DrawIndexed(indexCount, 0, 0);
				direct3DDeviceContext->RSSetState(CWcullMode_);
				direct3DDeviceContext->DrawIndexed(indexCount, 0, 0);
			}
			if (z == 5)
			{
				direct3DDeviceContext->RSSetState(CCWcullMode_);
				direct3DDeviceContext->DrawIndexed(indexCount, 0, 0);
				direct3DDeviceContext->RSSetState(CWcullMode_);
				direct3DDeviceContext->DrawIndexed(indexCount, 0, 0);
			}

			direct3DDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}

	//Text Rendering
	mSpriteBatch->Begin();
	wostringstream textLabel;
	textLabel << "W/A/S/D->Move\nQ/E->Fly\nB->Bind Pose\nR->Restart Anim\nSpace->Pause\nEsc->Quit";
	mSpriteFont->DrawString(mSpriteBatch.get(), textLabel.str().c_str(), mTextPosition);
	mSpriteBatch->End();
}

void CoreApp::UpdateUserInput()
{
	if (mKeyboard)
	{
		//Stop Advancing KeyFrames
		if (mKeyboard->WasKeyPressedThisFrame(DIK_SPACE))
		{
			for (auto aAnimPlayer : mAnimationPlayers)
			{
				if (aAnimPlayer->IsPlayingClip())
				{
					aAnimPlayer->PauseClip();
				}
				else
				{
					aAnimPlayer->ResumeClip();
				}
			}
		}

		//Reset And Start Playing From Bind Pose
		if (mKeyboard->WasKeyPressedThisFrame(DIK_R))
		{
			unsigned int modelIndex = 0;
			for (auto aAnimPlayer : mAnimationPlayers)
			{
				aAnimPlayer->StartClip(*(mSkinnedModels.at(modelIndex)->Animations().at(0)));
				++modelIndex;
			}
		}
		//Pause At Bind Pose
		if (mKeyboard->WasKeyPressedThisFrame(DIK_B))
		{
			unsigned int modelIndex = 0;
			for (auto aAnimPlayer : mAnimationPlayers)
			{
				aAnimPlayer->StartClip(*(mSkinnedModels.at(modelIndex)->Animations().at(0)));
				aAnimPlayer->PauseClip();
				++modelIndex;
			}
		}
	}
}


/*_____ENTERING TEMP ZONE_____*/// -> To Move to Renderer
void CoreApp::AnimationOneShot()
{
	//--Load Models-//
	if(loadingFromFBX && !loadingFromBIN)
	{
		//Opaque
		//Back Left
		mSkinnedModels.push_back(new Model(*mBaseApp, "Assets\\Geometry\\FBX\\Box_Idle.fbx"));
		//Front Left
		mSkinnedModels.push_back(new Model(*mBaseApp, "Assets\\Geometry\\FBX\\Box_Attack.fbx"));
		//Front Right
		mSkinnedModels.push_back(new Model(*mBaseApp, "Assets\\Geometry\\FBX\\Box_Jump.fbx"));
		//Back Right
		mSkinnedModels.push_back(new Model(*mBaseApp, "Assets\\Geometry\\FBX\\Box_Walk.fbx"));
		//Transparent
		//Back Left
		mSkinnedModels.push_back(new Model(*mBaseApp, "Assets\\Geometry\\FBX\\Box_Walk.fbx", true));
		//Idk anymore
		mSkinnedModels.push_back(new Model(*mBaseApp, "Assets\\Geometry\\FBX\\Box_Attack.fbx", true));

		//WriteOutToBinary();
	}

	//--Model Orientation-//
	//Opaque
	mModelsWorldMatrices.resize(mSkinnedModels.size());
	XMStoreFloat4x4(&mModelsWorldMatrices.at(0), XMMatrixTranslationFromVector(XMVectorSet(-3.0f, 0.0f, -2.0f, 1.0f)));
	XMStoreFloat4x4(&mModelsWorldMatrices.at(1), XMMatrixTranslationFromVector(XMVectorSet(-3.0f, 0.0f, 2.0f, 1.0f)));
	XMStoreFloat4x4(&mModelsWorldMatrices.at(2), XMMatrixTranslationFromVector(XMVectorSet(3.0f, 0.0f, 2.0f, 1.0f)));
	XMStoreFloat4x4(&mModelsWorldMatrices.at(3), XMMatrixTranslationFromVector(XMVectorSet(3.0f, 0.0f, -2.0f, 1.0f)));
	//Transparent
	XMStoreFloat4x4(&mModelsWorldMatrices.at(4), XMMatrixTranslationFromVector(XMVectorSet(0.0f, 0.0f, -2.0f, 1.0f)));
	XMStoreFloat4x4(&mModelsWorldMatrices.at(5), XMMatrixTranslationFromVector(XMVectorSet(0.0f, 0.0f, 2.0f, 1.0f)));


	//Load Materials
	for (size_t i = 0; i < mSkinnedModels.size(); i++)
	{
		mSkinnedMaterials.push_back(new SkinnedMaterial());
	}

	//Calc Total Mesh Count
	for (auto aModel : mSkinnedModels)
		mTotalNumMeshes += aModel->Meshes().size();


	//Buffer Resizing <- Not How The Renderer Handles It
	mVertexBuffers.resize(mTotalNumMeshes);
	mIndexBuffers.resize(mTotalNumMeshes);
	mIndexCounts.resize(mTotalNumMeshes);
	mColorTextures.resize(mTotalNumMeshes);

	//Constant Buffer Creation
	for (size_t i = 0; i < mSkinnedModels.size(); i++)
	{
		mModelMatrixConstantBuffers.push_back(new ConstantBuffer<cbPerObjectMatrixTemp>());
		mModelBoneTransformsConstantBuffers.push_back(new ConstantBuffer<cbPerObjectSkinningData>());
	}

	for (size_t i = 0; i < mSkinnedModels.size(); i++)
	{
		mModelMatrixConstantBuffers.at(i)->Initialize(mBaseApp->D3DDevice());
		mModelBoneTransformsConstantBuffers.at(i)->Initialize(mBaseApp->D3DDevice());
	}
	mScenePerFrameCameraConstBuffer.Initialize(mBaseApp->D3DDevice());

	//Mesh, Material, Buffer Loading
	for (auto aModel : mSkinnedModels)
	{
		for (UINT i = 0; i < aModel->Meshes().size(); i++)
		{
			Mesh* mesh = aModel->Meshes().at(i);

			ID3D11Buffer* vertexBuffer = nullptr;
			mSkinnedMaterials.at(i)->CreateVertexBuffer(mBaseApp->D3DDevice(), *mesh, &vertexBuffer);
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
	}


	//Create Models Animations
	for (size_t i = 0; i < mSkinnedModels.size(); i++)
	{
		mAnimationPlayers.push_back(new AnimationPlayer(*mBaseApp, *mSkinnedModels.at(i)));
	}

	//Playback Animations
	for (size_t i = 0; i < mAnimationPlayers.size(); i++)
	{
		mAnimationPlayers.at(i)->StartClip(*(mSkinnedModels.at(i)->Animations().at(0)));
	}
}




void CoreApp::BuildStuff()
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

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	mBaseApp->D3DDevice()->CreateSamplerState(&sd, &mAnisoSamplerState);

	//Rasterizer
	D3D11_RASTERIZER_DESC RSdesc;
	RSdesc.FillMode = D3D11_FILL_SOLID;
	RSdesc.CullMode = D3D11_CULL_BACK;
	RSdesc.FrontCounterClockwise = FALSE;
	RSdesc.DepthBias = 0;
	RSdesc.SlopeScaledDepthBias = 0.0f;
	RSdesc.DepthBiasClamp = 0.0f;
	RSdesc.DepthClipEnable = TRUE;
	RSdesc.ScissorEnable = FALSE;
	RSdesc.MultisampleEnable = FALSE;
	RSdesc.AntialiasedLineEnable = FALSE;
	mBaseApp->D3DDevice()->CreateRasterizerState(&RSdesc, &mDefaultRasterState);
	RSdesc.FillMode = D3D11_FILL_WIREFRAME;
	mBaseApp->D3DDevice()->CreateRasterizerState(&RSdesc, &mWireFrameRasterState);
	RSdesc.FillMode = D3D11_FILL_SOLID;
	RSdesc.FrontCounterClockwise = TRUE;
	mBaseApp->D3DDevice()->CreateRasterizerState(&RSdesc, &mCCWCullRasterState);
	RSdesc.CullMode = D3D11_CULL_NONE;
	mBaseApp->D3DDevice()->CreateRasterizerState(&RSdesc, &mNoCullRasterState);

	//Depth
	D3D11_DEPTH_STENCIL_DESC dssDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
	dssDesc.DepthEnable = FALSE;
	mBaseApp->D3DDevice()->CreateDepthStencilState(&dssDesc, &mNoDepthDSS);
	dssDesc.DepthEnable = TRUE;
	mBaseApp->D3DDevice()->CreateDepthStencilState(&dssDesc, &mDefaultDSS);
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	mBaseApp->D3DDevice()->CreateDepthStencilState(&dssDesc, &mLessEqualDSS);


	//Blend
	D3D11_BLEND_DESC BLdesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT());
	BLdesc.AlphaToCoverageEnable = FALSE;
	BLdesc.IndependentBlendEnable = FALSE;
	BLdesc.RenderTarget[0].BlendEnable = FALSE;
	BLdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BLdesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	BLdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BLdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BLdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BLdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BLdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	mBaseApp->D3DDevice()->CreateBlendState(&BLdesc, &mDefaultBlendState);

	BLdesc.RenderTarget[0].BlendEnable = TRUE;
	BLdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BLdesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	mBaseApp->D3DDevice()->CreateBlendState(&BLdesc, &mAlphaCoverageBlendState);

	BLdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BLdesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	mBaseApp->D3DDevice()->CreateBlendState(&BLdesc, &mAdditiveBlendState);

	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
	ZeroMemory(&renderTargetBlendDesc, sizeof(renderTargetBlendDesc));
	renderTargetBlendDesc.BlendEnable = true;
	renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_COLOR;
	renderTargetBlendDesc.DestBlend = D3D11_BLEND_BLEND_FACTOR; //Going to set the factor in the OM
	renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;			//Add the two colors together
	renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;		//Alpha Factor is straight 1.0f
	renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;	//Alpha Factor is straight 0.0f
	renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;	//Add the two alphas together
	renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	D3D11_BLEND_DESC transpBlendDesc;
	ZeroMemory(&transpBlendDesc, sizeof(transpBlendDesc));
	transpBlendDesc.AlphaToCoverageEnable = false;
	transpBlendDesc.RenderTarget[0] = renderTargetBlendDesc;

	//Create the Blend State
	mBaseApp->D3DDevice()->CreateBlendState(&transpBlendDesc, &mTransparentBlendState);


	//OTHER CULLERSSSSS
	D3D11_RASTERIZER_DESC cullModeDesc;
	ZeroMemory(&cullModeDesc, sizeof(cullModeDesc));
	cullModeDesc.FillMode = D3D11_FILL_SOLID;
	cullModeDesc.CullMode = D3D11_CULL_BACK;  //Dont draw back triangles
	cullModeDesc.FrontCounterClockwise = true;
	mBaseApp->D3DDevice()->CreateRasterizerState(&cullModeDesc, &CCWcullMode_);
	//CW
	cullModeDesc.FrontCounterClockwise = false;
	mBaseApp->D3DDevice()->CreateRasterizerState(&cullModeDesc, &CWcullMode_);

}

void CoreApp::ConstantBufferUpdate(UINT index)
{
	//SLOT 8//
	cbPerFrameLightCameraData scenePerFrameCamInfo;
	scenePerFrameCamInfo.AmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	scenePerFrameCamInfo.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	scenePerFrameCamInfo.LightPosition = XMFLOAT3(0.0f, 10.0f, 0.0f);
	scenePerFrameCamInfo.LightRadius = 500.0f;
	XMStoreFloat3(&scenePerFrameCamInfo.CameraPosition, mCamera->GetPositionXM());
	mScenePerFrameCameraConstBuffer.Data = scenePerFrameCamInfo;
	mScenePerFrameCameraConstBuffer.ApplyChanges(mBaseApp->D3DDevCon());
	auto cBufferPerFrame = mScenePerFrameCameraConstBuffer.Buffer();
	mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);
	mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_FRAME_LIGHT_CAMERA_SLOT, 1, &cBufferPerFrame);

	//SLOT 9//
	cbPerObjectMatrixTemp modelMatrixTempInfo;
	modelMatrixTempInfo.SpecularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	modelMatrixTempInfo.SpecularPower = 25.0f;
	XMMATRIX worldMatrix = XMLoadFloat4x4(&mModelsWorldMatrices.at(index));
	XMStoreFloat4x4(&modelMatrixTempInfo.World, worldMatrix);
	XMMATRIX wvp = worldMatrix * mCamera->GetViewMatrix() * mCamera->GetProjMatrix();
	XMStoreFloat4x4(&modelMatrixTempInfo.WorldViewProjection, wvp);
	mModelMatrixConstantBuffers.at(index)->Data = modelMatrixTempInfo;
	mModelMatrixConstantBuffers.at(index)->ApplyChanges(mBaseApp->D3DDevCon());
	auto cBufferPerObjMatrix = mModelMatrixConstantBuffers.at(index)->Buffer();
	mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_OBJECT_MATRIX_TEMP_REGISTER_SLOT, 1, &cBufferPerObjMatrix);
	mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_OBJECT_MATRIX_TEMP_REGISTER_SLOT, 1, &cBufferPerObjMatrix);

	//SLOT 10//
	cbPerObjectSkinningData modelSkinningData;
	copy(mAnimationPlayers.at(index)->BoneTransforms().begin(), mAnimationPlayers.at(index)->BoneTransforms().end(), modelSkinningData.BoneTransforms);
	mModelBoneTransformsConstantBuffers.at(index)->Data = modelSkinningData;
	mModelBoneTransformsConstantBuffers.at(index)->ApplyChanges(mBaseApp->D3DDevCon());
	auto cBufferPerObjBoneMatrices = mModelBoneTransformsConstantBuffers.at(index)->Buffer();
	mBaseApp->D3DDevCon()->VSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &cBufferPerObjBoneMatrices);
	mBaseApp->D3DDevCon()->PSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &cBufferPerObjBoneMatrices);
}
/*_____END TEMP ZONE_____*/// -> To Move to Renderer