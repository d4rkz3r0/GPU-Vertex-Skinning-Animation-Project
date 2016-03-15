#include "SkinnedMaterial.h"
#include "Mesh.h"
#include "Animation/Bone.h"
#include "../VertexStructs.h"
#include "../../../../Shared/SharedUtils.h"

RTTI_DEFINITIONS(SkinnedMaterial)

SkinnedMaterial::SkinnedMaterial() : Material()
{ }


void SkinnedMaterial::CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const
{
	const vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
	vector<XMFLOAT3>* textureCoordinates = mesh.TextureCoordinates().at(0);
	assert(textureCoordinates->size() == sourceVertices.size());
	const vector<XMFLOAT3>& normals = mesh.Normals();
	assert(normals.size() == sourceVertices.size());
	const vector<BoneVertexWeights>& boneWeights = mesh.BoneWeights();
	assert(boneWeights.size() == sourceVertices.size());

	vector<AnimationVertex> vertices;
	vertices.reserve(sourceVertices.size());
	for (UINT i = 0; i < sourceVertices.size(); i++)
	{
		XMFLOAT3 position = sourceVertices.at(i);
		XMFLOAT3 uv = textureCoordinates->at(i);
		XMFLOAT3 normal = normals.at(i);
		BoneVertexWeights vertexWeights = boneWeights.at(i);

		float weights[BoneVertexWeights::MaxBoneWeightsPerVertex];
		UINT indices[BoneVertexWeights::MaxBoneWeightsPerVertex];
		ZeroMemory(weights, sizeof(float) * ARRAYSIZE(weights));
		ZeroMemory(indices, sizeof(UINT) * ARRAYSIZE(indices));

		for (UINT i = 0; i < vertexWeights.Weights().size(); i++)
		{
			BoneVertexWeights::VertexWeight vertexWeight = vertexWeights.Weights().at(i);
			weights[i] = vertexWeight.Weight;
			indices[i] = vertexWeight.BoneIndex;
		}

		vertices.push_back(AnimationVertex(XMFLOAT4(position.x, position.y, position.z, 1.0f), XMFLOAT2(uv.x, uv.y), normal, XMUINT4(indices), XMFLOAT4(weights)));
	}

	CreateVertexBuffer(device, &vertices[0], vertices.size(), vertexBuffer);
}

void SkinnedMaterial::CreateVertexBuffer(ID3D11Device* device, AnimationVertex* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const
{
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.ByteWidth = VertexSize() * vertexCount;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexSubResourceData;
	ZeroMemory(&vertexSubResourceData, sizeof(vertexSubResourceData));
	vertexSubResourceData.pSysMem = vertices;
	device->CreateBuffer(&vertexBufferDesc, &vertexSubResourceData, vertexBuffer);
}

UINT SkinnedMaterial::VertexSize() const
{
	return sizeof(AnimationVertex);
}