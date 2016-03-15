#pragma once
#include "../../../../../Shared/SharedUtils.h"
#include "SceneNode.h"

class BoneVertexWeights
{
public:
	struct VertexWeight
	{
		float Weight;
		UINT BoneIndex;

		VertexWeight(float weight, UINT boneIndex) : Weight(weight), BoneIndex(boneIndex)
		{ }
	};

	const vector<VertexWeight>& Weights();

	void AddWeight(float weight, UINT boneIndex);

	static const UINT MaxBoneWeightsPerVertex = 4U;

private:
	vector<VertexWeight> mWeights;
};

class Bone : public SceneNode
{
	RTTI_DECLARATIONS(Bone, SceneNode)

public:
	UINT Index() const;
	void SetIndex(UINT index);

	const XMFLOAT4X4& OffsetTransform() const;
	XMMATRIX OffsetTransformMatrix() const;

	Bone(const string& name, UINT index, const XMFLOAT4X4& offsetTransform);

private:
	Bone();
	Bone(const Bone& rhs);
	Bone& operator=(const Bone& rhs);

	UINT mIndex;					//Index into the model's bone container
	XMFLOAT4X4 mOffsetTransform;	//Transforms from mesh space to bone space
};