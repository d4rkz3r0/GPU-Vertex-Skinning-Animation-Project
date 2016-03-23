#include "AnimatedMesh.h"
#include "../../../App/AppHelpers/Misc/Time.h"
#include "../../../App/AppHelpers/Misc/Exception.h"
#include "../../../App/AppHelpers/Cameras/Camera.h"
#include "../RendererHelpers/Animation/AssimpContainers.h"
#include "../RendererHelpers/Animation/AssimpHeaders.h"
#include "../VertexStructs.h"
#include <WICTextureLoader.h>


namespace Utility
{
	string GetFileExtension(const string& s)
	{
		size_t i = s.rfind('.', s.length());
		if (i != string::npos)
		{
			return(s.substr(i + 1, s.length() - i));
		}

		return("");
	}
}

enum class FILE_TYPE
{
	FBX_FILE = 'x',
	DAE_FILE = 'e'
};


AnimatedMesh::AnimatedMesh(ID3D11Device* pDevice, Camera& camera) : BaseMesh(camera), mDiffuseMapSRV(nullptr), mNormalMapSRV(nullptr), mHasDiffuse(true), mHasNormal(true), mTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	mDevice = pDevice;
	mDevice->GetImmediateContext(&mDeviceContext);

	AnimatedMesh::Initialize();
}

AnimatedMesh::~AnimatedMesh()
{
	AnimatedMesh::Shutdown();
}

void AnimatedMesh::Initialize()
{
	SetModelOrientation();
	
	mVertexBuffer.Initialize(mDevice);
	mIndexBuffer.Initialize(mDevice);
	mCB0.Initialize(mDevice);
	mCB1.Initialize(mDevice);
	mCB2.Initialize(mDevice);
}

void AnimatedMesh::Shutdown()
{
	BaseMesh::Shutdown();
	mVertexBuffer.Destroy();
	mIndexBuffer.Destroy();
}

void AnimatedMesh::Load(const string& fileName)
{
	Shutdown();
	LoadModel(fileName);
}

void AnimatedMesh::Update(Time& deltaTime) { }


void AnimatedMesh::LoadTextures(const aiMesh* mesh, aiMaterial** mMaterials)
{
	aiString texturePath;
	wostringstream diffuseTextureFullPath;
	wostringstream normalTextureFullPath;

	//Diffuse Map
	if (AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_DIFFUSE(0), &texturePath))
	{
		mHasDiffuse = true;
		diffuseTextureFullPath << mTextureFilePath.c_str() << L"Teddy_D.png";
		HRESULT hr = CreateWICTextureFromFile(mDevice, mDeviceContext, diffuseTextureFullPath.str().c_str(), nullptr, &mDiffuseMapSRV);
		if (FAILED(hr))
		{
			throw Exception("AnimatedMesh::LoadTextures(), MeshFile contains a ColorMap, but I can't extract the proper filePath!", hr);
		}
	}

	////Normal Map
	//if (AI_SUCCESS == aiGetMaterialString(mMaterials[mesh->mMaterialIndex], AI_MATKEY_TEXTURE_NORMALS(0), &texturePath))
	//{
		mHasNormal = true;
		normalTextureFullPath << mTextureFilePath.c_str() << L"Teddy_N.png";
		HRESULT hr = CreateWICTextureFromFile(mDevice, mDeviceContext, normalTextureFullPath.str().c_str(), nullptr, &mNormalMapSRV);
		if (FAILED(hr))
		{
			throw Exception("AnimatedMesh::LoadTextures(), MeshFile contains a NormalMap, but I can't extract the proper filePath!", hr);
		}
	//}
}

void AnimatedMesh::LoadMaterialScalars(const aiMaterial* meshMaterial)
{
	//If the meshFile provides material info, extract it, otherwise default the values.
	mMaterialInfo.mDiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor4D*>(&mMaterialInfo.mDiffuseColor));
	mMaterialInfo.mSpecularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor4D*>(&mMaterialInfo.mSpecularColor));
	mMaterialInfo.mAmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_AMBIENT, reinterpret_cast<aiColor4D*>(&mMaterialInfo.mAmbientColor));
	mMaterialInfo.mEmissiveColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_EMISSIVE, reinterpret_cast<aiColor4D*>(&mMaterialInfo.mEmissiveColor));
	mMaterialInfo.mSpecularPower = 10.0f;
	aiGetMaterialFloat(meshMaterial, AI_MATKEY_SHININESS_STRENGTH, &mMaterialInfo.mSpecularPower);
}

VertexBuffer<AnimationVertex>& AnimatedMesh::GetVertexBuffer()
{
	return mVertexBuffer;
}

IndexBuffer& AnimatedMesh::GetIndexBuffer()
{
	return mIndexBuffer;
}

void AnimatedMesh::LoadModel(const string& fileName)
{
	//Cache Mesh Info
	mFileName = fileName;
	mFileType = Utility::GetFileExtension(fileName);
	auto fileTypeLocal = mFileType.at(2);

	//File Directory
	switch (fileTypeLocal)
	{
		case char(FILE_TYPE::FBX_FILE) :
		{
			mFilePath = "Assets\\Geometry\\FBX\\" + mFileName;
			break;
		}
		case char(FILE_TYPE::DAE_FILE) :
		{
			mFilePath = "Assets\\Geometry\\DAE\\" + mFileName;
			break;
		}
		default:
		{
			break;
		}
	}

	//Assimp Load
	UINT importFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded;
	const aiScene* scene = aiImportFile(mFilePath.c_str(), importFlags);

	if (scene == nullptr)
	{
		throw Exception("AnimatedMesh::LoadModel(), File not found!");
	}

	//Init
	size_t numVertices(0);
	size_t currVertex(0);
	size_t numIndices(0);
	size_t currIndex(0);
	bool bonesPresent = false;

	//Obtain Vertex and Indices Count
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		numVertices += scene->mMeshes[i]->mNumVertices;
		numIndices += scene->mMeshes[i]->mNumFaces * 3;
		if (scene->mMeshes[i]->HasBones())
		{
			bonesPresent = true;
		}
	}
	//Early Out
	if (!bonesPresent)
	{
		aiReleaseImport(scene);
		throw Exception("AnimatedMesh::LoadModel(), No Bones detected!");
	}

	//Determine Index Buffers ByteWidth
	if (numVertices >= 65536)
	{
		mIndexBuffer.SetStride(4);
	}

	//Vector Prep
	mVertices.resize(numVertices);
	mIndices.resize((mIndexBuffer.GetStride() / 2) * numIndices);
	vector<AnimationVertex> tempVertices(numVertices);
	vector<string> boneStrings;

	//Iterate through a Scene's Meshes, stop loading as soon as 1 single
	//invalid mesh is found. Teddy only has one mesh.
	for (size_t i = 0; i < scene->mNumMeshes; i++)
	{
		//Validate MeshFile
		const aiMesh* mesh = scene->mMeshes[i];
		if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
		{
			throw Exception("AnimatedMesh::LoadModel(), MeshFile's Topology doesn't consist of Triangles!");
		}
		if (!mesh->HasTextureCoords(0))
		{
			throw Exception("AnimatedMesh::LoadModel(), MeshFile lacks UVs!");
		}
		if (!mesh->HasTangentsAndBitangents())
		{
			throw Exception("AnimatedMesh::LoadModel(), MeshFile lacks Tangent Info!");
		}

		//Cache Mesh Info
		mNumVertices = mesh->mNumVertices;
		mNumIndices = mesh->mNumFaces * 3;
		mStartingIndex = static_cast<uint32_t>(currIndex);

		//Populate Vertices
		for (size_t x = 0; x < mesh->mNumVertices; x++)
		{
			//Position
			mVertices[x + currVertex] = tempVertices[x + currVertex].Position = *reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[x]);
			//UVs
			tempVertices[x + currVertex].TextureCoordinates = *reinterpret_cast<XMFLOAT2*>(&mesh->mTextureCoords[0][x]);
			//Normals
			tempVertices[x + currVertex].Normal = *reinterpret_cast<XMFLOAT3*>(&mesh->mNormals[x]);
			//Tangents
			tempVertices[x + currVertex].Tangent = *reinterpret_cast<XMFLOAT3*>(&mesh->mTangents[x]);
		}

		//Populate Indices Based on ByteWidth
		if (mIndexBuffer.GetStride() == 4)
		{
			uint32_t* indexBufferData = reinterpret_cast<uint32_t*>(&mIndices[currIndex]);
			for (size_t x = 0; x < mesh->mNumFaces; ++x)
			{
				for (size_t a = 0; a < 3; ++a)
				{
					*indexBufferData++ = static_cast<uint32_t>(mesh->mFaces[x].mIndices[a] + currVertex);
				}
			}
		}
		else
		{
			uint16_t* indexBufferData = reinterpret_cast<uint16_t*>(&mIndices[currIndex]);
			for (size_t x = 0; x < mesh->mNumFaces; ++x)
			{
				for (size_t a = 0; a < 3; ++a)
				{
					*indexBufferData++ = static_cast<uint16_t>(mesh->mFaces[x].mIndices[a] + currVertex);
				}
			}
		}

		//Iterate Through Mesh Files Bones
		for (size_t a = 0; a < mesh->mNumBones; a++)
		{
			//Init
			const aiBone* currBone = mesh->mBones[a];
			size_t boneIndex = -1;

			//Check for unique bone names
			for (size_t bIndex = 0; bIndex < boneStrings.size(); bIndex++)
			{
				string boneName = currBone->mName.data;
				if (boneName == boneStrings[bIndex])
				{
					boneIndex = bIndex;
					break;
				}
			}

			//Add unique bone name
			if (boneIndex == -1)
			{
				boneIndex = boneStrings.size();
				boneStrings.push_back(currBone->mName.data);
			}

			//Extract Bone Indices and their Weights
			for (size_t b = 0; b < currBone->mNumWeights; b++)
			{
				if (tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.x <= 0.f)
				{
					tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneIndices.x = static_cast<float>(boneIndex);
					tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.x = currBone->mWeights[b].mWeight;
				}
				else if (tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.y <= 0.f)
				{
					tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneIndices.y = static_cast<float>(boneIndex);
					tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.y = currBone->mWeights[b].mWeight;
				}
				else if (tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.z <= 0.f)
				{
					tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneIndices.z = static_cast<float>(boneIndex);
					tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.z = currBone->mWeights[b].mWeight;
				}
				else if (tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.w <= 0.f)
				{
					tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneIndices.w = static_cast<float>(boneIndex);
					tempVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.w = currBone->mWeights[b].mWeight;
				}
			}
		}

		//Load Textures & Get Material Info
		LoadTextures(mesh, scene->mMaterials);
		LoadMaterialScalars(scene->mMaterials[mesh->mMaterialIndex]);

		//Continue
		currVertex += mesh->mNumVertices;
		currIndex += mesh->mNumFaces * 3;
	}

	//Load Animation Data
	mAnimator.Init(scene);

	//Final Check
	if (currVertex == 0)
	{
		throw Exception("AnimatedMesh::LoadModel(), Nothing was loaded from the Mesh File!");
	}

	//Buffer Creation
	mVertexBuffer.CreateBuffer(currVertex, sizeof(AnimationVertex), &tempVertices[0]);
	mIndexBuffer.CreateBuffer(currIndex, &mIndices[0]);

	//Release Assimp ScenePtr
	aiReleaseImport(scene);
}

void AnimatedMesh::SaveModel(const string& fileName)
{

}

void AnimatedMesh::Draw(float deltaTime)
{
	
	mDeviceContext->IASetPrimitiveTopology(mTopology);
	mVertexBuffer.BindBuffer();
	mIndexBuffer.BindBuffer();

	if (HasDiffuseMap()) { mDeviceContext->PSSetShaderResources(0, 1, &mDiffuseMapSRV); }
	if (HasNormalMap()) { mDeviceContext->PSSetShaderResources(1, 1, &mNormalMapSRV); }

	UpdateConstantBuffers(deltaTime);

	mDeviceContext->DrawIndexed(mNumIndices, mStartingIndex, 0);
}

void AnimatedMesh::UpdateConstantBuffers(float deltaTime)
{
	//-Constant Buffer Updates-//
	cbPerObjectMatrix perObjCB0;
	XMVECTOR detVec;
	XMStoreFloat4x4(&perObjCB0.mWorldMatrix, XMMatrixTranspose(GetWorld()));
	XMStoreFloat4x4(&perObjCB0.mViewProjMatrix, XMMatrixTranspose(mCamera->GetViewProj()));
	XMStoreFloat4x4(&perObjCB0.mInvTransWorldMatrix, XMMatrixInverse(&detVec, XMMatrixTranspose(GetWorld())));
	mCB0.Data = perObjCB0;
	mCB0.ApplyChanges(mDeviceContext);
	auto CB0 = mCB0.Buffer();
	mDeviceContext->VSSetConstantBuffers(CB_PER_OBJECT_MATRIX_REGISTER_SLOT, 1, &CB0);
	mDeviceContext->PSSetConstantBuffers(CB_PER_OBJECT_MATRIX_REGISTER_SLOT, 1, &CB0);

	//Material Scalars
	mCB1.Data = mMaterialInfo;
	mCB1.ApplyChanges(mDeviceContext);
	auto CB1 = mCB1.Buffer();
	mDeviceContext->VSSetConstantBuffers(CB_PER_OBJECT_MATERIAL_REGISTER_SLOT, 1, &CB1);
	mDeviceContext->PSSetConstantBuffers(CB_PER_OBJECT_MATERIAL_REGISTER_SLOT, 1, &CB1);

	cbPerObjectSkinningData perObjCB2;
	copy(mAnimator.GetTransforms(deltaTime).begin(), mAnimator.GetTransforms(deltaTime).end(), perObjCB2.BoneTransforms);
	for (size_t i = 0; i < ARRAYSIZE(perObjCB2.BoneTransforms); i++)
	{
		XMStoreFloat4x4(&perObjCB2.BoneTransforms[i], XMMatrixTranspose(XMLoadFloat4x4(&perObjCB2.BoneTransforms[i])));
	}
	mCB2.Data = perObjCB2;
	mCB2.ApplyChanges(mDeviceContext);
	auto CB2 = mCB2.Buffer();
	mDeviceContext->VSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &CB2);
	mDeviceContext->PSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &CB2);
}

void AnimatedMesh::SetModelOrientation(float xPos, float yPos, float zPos, float xScale, float yScale, float zScale, float xRot, float yRot, float zRot)
{
	SetScaling(XMMatrixScaling(xScale, yScale, zScale));
	SetRotation(XMMatrixMultiply(XMMatrixRotationX(xRot), XMMatrixMultiply(XMMatrixRotationY(yRot), XMMatrixRotationZ(zRot))));
	SetTranslation(XMMatrixTranslationFromVector(XMVectorSet(xPos, yPos, zPos, 1.0f)));
}