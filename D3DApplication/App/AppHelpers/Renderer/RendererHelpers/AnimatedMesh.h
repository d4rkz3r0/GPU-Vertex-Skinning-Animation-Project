#pragma once
#include "../RendererHelpers/Animation/AssimpContainers.h"
#include "../RendererHelpers/CBs/ConstantBuffer.h"
#include "../RendererHelpers/CBs/CBRefs.h"
#include "../../../Shared/SharedUtils.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "BaseMesh.h"

//Forward Declarations
class Camera;
class Time;

class AnimatedMesh : public BaseMesh
{
public:
	AnimatedMesh(ID3D11Device* pDevice, Camera& camera);
	~AnimatedMesh();

	virtual void Initialize() override;
	virtual void Shutdown() override;
	virtual void Load(const string& fileName) override;
	virtual void Update(Time& deltaTime) override;
	virtual void Draw(float deltaTime) override;
	virtual void UpdateConstantBuffers(float deltaTime) override;

	//Utility
	bool HasNormalMap() { return mHasNormal; }
	bool HasDiffuseMap() { return mHasDiffuse; }
	void SetModelOrientation(float xPos = 0.0f, float yPos = 0.0f, float zPos = 0.0f, 
						     float xScale = 15.0f, float yScale = 15.0f, float zScale = 15.0f,
							 float xRot = 0.0f, float yRot = 0.0f, float zRot = 0.0f);

	//Accessors
	VertexBuffer<AnimationVertex>& GetVertexBuffer();
	IndexBuffer& GetIndexBuffer();

private:
	//D3D11 Objects
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;

	//Private Helpers
	void LoadModel(const string& fileName);
	void SaveModel(const string& fileName);
	void LoadTextures(const aiMesh* mesh, aiMaterial** mMaterials);
	void LoadMaterialScalars(const aiMaterial* meshMaterial);

	//Animation Encapsulation
	SceneAnimator mAnimator;

	//Buffers
	VertexBuffer<AnimationVertex> mVertexBuffer;
	IndexBuffer mIndexBuffer;
	//GPU Side Constant Buffers
	ConstantBuffer<cbPerObjectMatrix> mCB0;
	ConstantBuffer<cbPerObjectMaterial> mCB1;
	ConstantBuffer<cbPerObjectSkinningData> mCB2;
	//CPU Side Constant Buffers
	cbPerObjectMaterial mMaterialInfo;

	//Textures -> Should Make a Class for these...
	ID3D11ShaderResourceView* mDiffuseMapSRV;
	ID3D11ShaderResourceView* mNormalMapSRV;
	bool mHasDiffuse;
	bool mHasNormal;

	//Drawing
	vector<XMFLOAT3> mVertices;
	vector<uint16_t> mIndices;
	UINT mNumVertices;
	UINT mNumIndices;
	UINT mStartingIndex;

	//Misc
	D3D11_PRIMITIVE_TOPOLOGY mTopology;
};