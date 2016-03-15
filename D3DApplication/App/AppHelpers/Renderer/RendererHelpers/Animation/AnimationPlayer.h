#pragma once
#include "../../../Components/BaseComponent.h"

//Forward Declarations
class Time;
class Model;
class SceneNode;
class AnimationClip;

class AnimationPlayer : BaseComponent
{
	RTTI_DECLARATIONS(AnimationPlayer, BaseComponent)

public:
	AnimationPlayer(BaseApp& baseApp, Model& model, bool interpolationEnabled = true);

	const Model& GetModel() const;
	const AnimationClip* CurrentClip() const;
	float CurrentTime() const;
	UINT CurrentKeyframe() const;
	const vector<XMFLOAT4X4>& BoneTransforms() const;

	bool InterpolationEnabled() const;
	bool IsPlayingClip() const;
	bool IsClipLooped() const;

	void SetInterpolationEnabled(bool interpolationEnabled);

	void StartClip(AnimationClip& clip);
	void PauseClip();
	void ResumeClip();
	virtual void Update(const Time& Time) override;
	void SetCurrentKeyFrame(UINT keyframe);

private:
	AnimationPlayer();
	AnimationPlayer(const AnimationPlayer& rhs);
	AnimationPlayer& operator=(const AnimationPlayer& rhs);

	void GetBindPose(SceneNode& sceneNode);
	void GetBindPoseBottomUp(SceneNode& sceneNode);
	void GetPose(float time, SceneNode& sceneNode);
	void GetPoseAtKeyframe(UINT keyframe, SceneNode& sceneNode);
	void GetInterpolatedPose(float time, SceneNode& sceneNode);

	Model* mModel;
	AnimationClip* mCurrentClip;
	float mCurrentTime;
	UINT mCurrentKeyframe;
	map<SceneNode*, XMFLOAT4X4> mToRootTransforms;
	vector<XMFLOAT4X4> mFinalTransforms;
	XMFLOAT4X4 mInverseRootTransform;
	bool mInterpolationEnabled;
	bool mIsPlayingClip;
	bool mIsClipLooped;
};