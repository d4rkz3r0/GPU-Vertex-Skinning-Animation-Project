#pragma once
#include <DirectXMath.h>
using namespace DirectX;

struct NormalVertex
{
	NormalVertex() {}

	NormalVertex(const XMFLOAT3& pos, const XMFLOAT3& tan, const XMFLOAT3& bnorm, const XMFLOAT3& norm, const XMFLOAT2& uv) :
		position(pos),
		tangent(tan),
		biNormal(bnorm),
		normal(norm),
		texCoords(uv) {}

	NormalVertex(float px, float py, float pz,
		float tx, float ty, float tz,
		float bx, float by, float bz,
		float nx, float ny, float nz,
		float u, float v) :
		position(px, py, pz),
		tangent(tx, ty, tz),
		biNormal(bx, by, bz),
		normal(nx, ny, nz),
		texCoords(u, v) { }

	XMFLOAT3 position;
	XMFLOAT3 tangent;
	XMFLOAT3 biNormal;
	XMFLOAT3 normal;
	XMFLOAT2 texCoords;
};

struct AnimationVertex
{
	AnimationVertex() { }

	AnimationVertex(const XMFLOAT4& position, const XMFLOAT2& textureCoordinates, const XMFLOAT3& normal, const XMUINT4& boneIndices, const XMFLOAT4& boneWeights) :
		Position(position),
		TextureCoordinates(textureCoordinates),
		Normal(normal),
		BoneIndices(boneIndices),
		BoneWeights(boneWeights) { }

	XMFLOAT4 Position;
	XMFLOAT2 TextureCoordinates;
	XMFLOAT3 Normal;
	XMUINT4 BoneIndices;
	XMFLOAT4 BoneWeights;
};