#pragma once
#include "../../../../../Shared/SharedUtils.h"

//Forward Declarations
struct aiAnimation;
class Bone;
class BoneAnimation;

class AnimationClip
{
	friend class Model;

public:
	~AnimationClip();

	const string& Name() const;
	float Duration() const;
	float TicksPerSecond() const;
	const vector<BoneAnimation*>& BoneAnimations() const;
	const map<Bone*, BoneAnimation*>& BoneAnimationsByBone() const;
	const UINT KeyframeCount() const;

	UINT GetTransform(float time, Bone& bone, XMFLOAT4X4& transform) const;
	void GetTransforms(float time, vector<XMFLOAT4X4>& boneTransforms) const;

	void GetTransformAtKeyframe(UINT keyframe, Bone& bone, XMFLOAT4X4& transform) const;
	void GetTransformsAtKeyframe(UINT keyframe, vector<XMFLOAT4X4>& boneTransforms) const;

	void GetInteropolatedTransform(float time, Bone& bone, XMFLOAT4X4& transform) const;
	void GetInteropolatedTransforms(float time, vector<XMFLOAT4X4>& boneTransforms) const;

private:
	AnimationClip(Model& model, aiAnimation& animation);

	AnimationClip();
	AnimationClip(const AnimationClip& rhs);
	AnimationClip& operator=(const AnimationClip& rhs);

	string mName;
	float mDuration;
	float mTicksPerSecond;
	vector<BoneAnimation*> mBoneAnimations;
	map<Bone*, BoneAnimation*> mBoneAnimationsByBone;
	UINT mKeyframeCount;
};