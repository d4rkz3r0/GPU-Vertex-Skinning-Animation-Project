#pragma once
#include "../../../Shared/SharedUtils.h"
#include "SkinnedMaterial.h"

//Forward Declarations
struct aiNode;
class Game;
class Mesh;
class Bone;
class BaseApp;
class ModelMaterial;
class AnimationClip;
class SceneNode;

class Model
{
	friend class Mesh;

public:
	Model(BaseApp& baseApp, const string& filename, bool flipUVs = false);
	~Model();

	BaseApp& GetBaseApp();
	bool HasMeshes() const;
	bool HasMaterials() const;
	bool HasAnimations() const;

	const vector<Mesh*>& Meshes() const;
	const vector<ModelMaterial*>& Materials() const;
	const vector<AnimationClip*>& Animations() const;
	const map<string, AnimationClip*>& AnimationsbyName() const;
	const vector<Bone*> Bones() const;
	const map<string, UINT> BoneIndexMapping() const;
	SceneNode* RootNode();

private:
	Model(const Model& rhs);
	Model& operator=(const Model& rhs);

	SceneNode* BuildSkeleton(aiNode& node, SceneNode* parentSceneNode);
	void ValidateModel();
	void DeleteSceneNode(SceneNode* sceneNode);

	BaseApp& mBaseApp;
	vector<Mesh*> mMeshes;
	vector<ModelMaterial*> mMaterials;
	vector<AnimationClip*> mAnimations;
	map<string, AnimationClip*> mAnimationsByName;
	vector<Bone*> mBones;
	map<string, UINT> mBoneIndexMapping;
	SceneNode* mRootNode;
};