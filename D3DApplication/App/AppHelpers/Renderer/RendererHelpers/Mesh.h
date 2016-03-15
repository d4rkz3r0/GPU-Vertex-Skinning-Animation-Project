#pragma once
#include "../../../../Shared/SharedUtils.h"
#include "BufferWrapper.h"

//Forward Declarations
struct aiMesh;
class Material;
class ModelMaterial;
class BoneVertexWeights;

class Mesh
{
	friend class Model;

public:
	~Mesh();

	Model& GetModel();
	ModelMaterial* GetMaterial();
	const string& Name() const;

	const vector<XMFLOAT3>& Vertices() const;
	const vector<XMFLOAT3>& Normals() const;
	const vector<XMFLOAT3>& Tangents() const;
	const vector<XMFLOAT3>& BiNormals() const;
	const vector<vector<XMFLOAT3>*>& TextureCoordinates() const;
	const vector<vector<XMFLOAT4>*>& VertexColors() const;
	UINT FaceCount() const;
	const vector<UINT>& Indices() const;
	const vector<BoneVertexWeights>& BoneWeights() const;

	BufferWrapper& VertexBuffer();
	BufferWrapper& IndexBuffer();

	bool HasCachedVertexBuffer() const;
	bool HasCachedIndexBuffer() const;

	void CreateIndexBuffer(ID3D11Buffer** indexBuffer);
	void CreateCachedVertexAndIndexBuffers(ID3D11Device& device, const Material& material);

private:
	Mesh(Model& model, aiMesh& mesh);
	Mesh(const Mesh& rhs);
	Mesh& operator=(const Mesh& rhs);

	Model& mModel;
	ModelMaterial* mMaterial;
	string mName;
	vector<XMFLOAT3> mVertices;
	vector<XMFLOAT3> mNormals;
	vector<XMFLOAT3> mTangents;
	vector<XMFLOAT3> mBiNormals;
	vector<vector<XMFLOAT3>*> mTextureCoordinates;
	vector<vector<XMFLOAT4>*> mVertexColors;
	UINT mFaceCount;
	vector<UINT> mIndices;
	vector<BoneVertexWeights> mBoneWeights;

	BufferWrapper mVertexBuffer;
	BufferWrapper mIndexBuffer;
};