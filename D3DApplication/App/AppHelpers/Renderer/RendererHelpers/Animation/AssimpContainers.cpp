#include "AssimpContainers.h"
#include "../../../Shared/SharedUtils.h"


//aiMatrix -> XMFLOAT4X4 
void TransformMatrix(XMFLOAT4X4& out, const aiMatrix4x4& in)
{
	out._11 = in.a1;
	out._12 = in.a2;
	out._13 = in.a3;
	out._14 = in.a4;

	out._21 = in.b1;
	out._22 = in.b2;
	out._23 = in.b3;
	out._24 = in.b4;

	out._31 = in.c1;
	out._32 = in.c2;
	out._33 = in.c3;
	out._34 = in.c4;

	out._41 = in.d1;
	out._42 = in.d2;
	out._43 = in.d3;
	out._44 = in.d4;
}


AnimationEvaluator::AnimationEvaluator(const aiAnimation* pAnim)
{
	//Init
	mLastTime = 0.0f;
	TicksPerSecond = static_cast<float>(pAnim->mTicksPerSecond != 0.0f ? pAnim->mTicksPerSecond : 100.0f);
	Duration = static_cast<float>(pAnim->mDuration);
	Name = string(pAnim->mName.data, pAnim->mName.length);

	//If the MeshFile doesn't provide a Name for the animation, assign one.
	if (Name.size() == 0)
	{
		Name = "Animation" + to_string(AnimationIndex);
	}

	//Animation Prep
	Channels.resize(pAnim->mNumChannels);
	mLastPositions.resize(pAnim->mNumChannels, make_tuple(0, 0, 0));

	//Extract Name, Position, Rotation, Scaling Info per Animation Channel
	for (unsigned int a = 0; a < pAnim->mNumChannels; a++)
	{
		Channels[a].Name = pAnim->mChannels[a]->mNodeName.data;

		for (unsigned int i(0); i < pAnim->mChannels[a]->mNumPositionKeys; i++)
		{
			Channels[a].mPositionKeys.push_back(pAnim->mChannels[a]->mPositionKeys[i]);
		}
		for (unsigned int i(0); i < pAnim->mChannels[a]->mNumRotationKeys; i++)
		{
			Channels[a].mRotationKeys.push_back(pAnim->mChannels[a]->mRotationKeys[i]);
		}
		for (unsigned int i(0); i < pAnim->mChannels[a]->mNumScalingKeys; i++)
		{
			Channels[a].mScalingKeys.push_back(pAnim->mChannels[a]->mScalingKeys[i]);
		}
	}
}


unsigned int AnimationEvaluator::GetFrameIndexAt(float ptime)
{
	ptime *= TicksPerSecond;

	//[0.0f < - > 1.0f]
	float time = 0.0f;
	if (Duration > 0.0)
	{
		time = fmod(ptime, Duration);
	}
	float percent = time / Duration;

	//Reverse the Animation by simply inverting the percent
	if (!PlayAnimationForward)
	{
		percent = (percent - 1.0f) * -1.0f;
	}
	return static_cast<unsigned int>((static_cast<float>(Transforms.size()) * percent));
}

//Given a time, calculate Bone's local transformation matrix
void AnimationEvaluator::Evaluate(float pTime, map<string, Bone*>& bones)
{
	pTime *= TicksPerSecond;

	//[0.0f < - > 1.0f]
	float time = 0.0f;
	if (Duration > 0.0)
	{
		time = fmod(pTime, Duration);
	}

	//Calc Transformations
	for (unsigned int a = 0; a < Channels.size(); a++)
	{
		const AnimationChannel* channel = &Channels[a];
		map<string, Bone*>::iterator bonenode = bones.find(channel->Name);

		//Couldn't find the Bone in this channel? Nexttt...
		if (bonenode == bones.end())
		{
			continue;
		}

		//Position
		aiVector3D presentPosition(0.0f, 0.0f, 0.0f);
		if (channel->mPositionKeys.size() > 0)
		{
			//Searching Optimization, no need to start from the beginning each time.
			unsigned int currFrame = (time >= mLastTime) ? get<0>(mLastPositions[a]) : 0;

			while (currFrame < channel->mPositionKeys.size() - 1)
			{
				if (time < channel->mPositionKeys[currFrame + 1].mTime)
				{
					break;
				}
				currFrame++;
			}

			//Interpolation Prep
			unsigned int nextFrame = (currFrame + 1) % channel->mPositionKeys.size();
			const aiVectorKey& key = channel->mPositionKeys[currFrame];
			const aiVectorKey& nextKey = channel->mPositionKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;


			if (diffTime < 0.0)
			{
				diffTime += Duration;
			}

			if (diffTime > 0.0)
			{
				//Calculate That Ratio
				float ratio = float((time - key.mTime) / diffTime);
				//Now Lerp
				presentPosition = key.mValue + (nextKey.mValue - key.mValue) * ratio;
			}
			else
			{
				presentPosition = key.mValue;
			}
			//Keep Frame Updated
			get<0>(mLastPositions[a]) = currFrame;
		}


		//Rotation
		aiQuaternion presentRotation(1, 0, 0, 0);
		if (channel->mRotationKeys.size() > 0)
		{
			//Searching Optimization, no need to start from the beginning each time.
			unsigned int currFrame = (time >= mLastTime) ? get<1>(mLastPositions[a]) : 0;

			while (currFrame < channel->mRotationKeys.size() - 1)
			{
				if (time < channel->mRotationKeys[currFrame + 1].mTime)
				{
					break;
				}
				currFrame++;
			}

			//Interpolation Prep
			unsigned int nextFrame = (currFrame + 1) % channel->mRotationKeys.size();
			const aiQuatKey& key = channel->mRotationKeys[currFrame];
			const aiQuatKey& nextKey = channel->mRotationKeys[nextFrame];
			double diffTime = nextKey.mTime - key.mTime;

			if (diffTime < 0.0)
			{
				diffTime += Duration;
			}
			if (diffTime > 0.0)
			{
				//Lerp
				float ratio = float((time - key.mTime) / diffTime);
				aiQuaternion::Interpolate(presentRotation, key.mValue, nextKey.mValue, ratio);
			}
			else
			{
				presentRotation = key.mValue;
			}
			//Keep Frame Updated
			get<1>(mLastPositions[a]) = currFrame;
		}

		//Scaling
		aiVector3D presentScaling(1, 1, 1);
		if (channel->mScalingKeys.size() > 0)
		{
			//Searching Optimization, no need to start from the beginning each time.
			unsigned int currFrame = (time >= mLastTime) ? get<2>(mLastPositions[a]) : 0;

			while (currFrame < channel->mScalingKeys.size() - 1)
			{
				if (time < channel->mScalingKeys[currFrame + 1].mTime)
				{
					break;
				}
				currFrame++;
			}

			//No Lerping
			presentScaling = channel->mScalingKeys[currFrame].mValue;
			//Keep Frame Updated
			get<2>(mLastPositions[a]) = currFrame;
		}

		//-Building Bone's Local Transformation Matrix-//
		//1) Start with Rotation Matrix from Quaternion
		aiMatrix4x4 aiMatrix = aiMatrix4x4(presentRotation.GetMatrix());

		//2) Integrate Scaling and Position
		aiMatrix.a1 *= presentScaling.x; aiMatrix.b1 *= presentScaling.x; aiMatrix.c1 *= presentScaling.x;
		aiMatrix.a2 *= presentScaling.y; aiMatrix.b2 *= presentScaling.y; aiMatrix.c2 *= presentScaling.y;
		aiMatrix.a3 *= presentScaling.z; aiMatrix.b3 *= presentScaling.z; aiMatrix.c3 *= presentScaling.z;
		aiMatrix.a4 = presentPosition.x; aiMatrix.b4 = presentPosition.y; aiMatrix.c4 = presentPosition.z;

		//3) Transpose it
		aiMatrix.Transpose();

		//4) Set it
		TransformMatrix(bonenode->second->LocalTransform, aiMatrix);
	}
	//Keep Time Updated
	mLastTime = time;
}


void SceneAnimator::Release()
{
	CurrentAnimIndex = -1;
	Animations.clear();

	delete Skeleton;  //Recursive
	Skeleton = nullptr;
}


void SceneAnimator::Init(const aiScene* pScene)
{
	//Early Out
	if (!pScene->HasAnimations())
	{
		return;
	}

	//Init
	Release();
	Skeleton = CreateBoneTree(pScene->mRootNode, NULL);
	ExtractAnimations(pScene);

	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
	{
		const aiMesh* mesh = pScene->mMeshes[i];

		for (unsigned int n = 0; n < mesh->mNumBones; ++n)
		{
			const aiBone* currBone = mesh->mBones[n];

			//Search for the Bone in the Skeleton by Name
			map<string, Bone*>::iterator found = BonesByName.find(currBone->mName.data);

			//Got a hit, but is it a new bone?
			if (found != BonesByName.end())
			{
				bool skip = false;

				//Loop through bones
				for (size_t j = 0; j < Bones.size(); j++)
				{
					//Compare Name
					string bname = currBone->mName.data;
					if (Bones[j]->Name == bname)
					{
						//Not a unique bone, GTFO.
						skip = true;
						break;
					}
				}

				//Unique Bone
				if (!skip)
				{
					//Store It's Matrix
					TransformMatrix(found->second->Offset, currBone->mOffsetMatrix);
					XMStoreFloat4x4(&found->second->Offset, XMMatrixTranspose(XMLoadFloat4x4(&found->second->Offset)));

					//Add it to the list
					Bones.push_back(found->second);
					BonesToIndex[found->first] = static_cast<unsigned int>(Bones.size()) - 1;
				}
			}
		}
	}

	//Temp Matrices Prep
	Transforms.resize(Bones.size());
	float timeStep = 1.0f / 30.0f;

	for (size_t i = 0; i < Animations.size(); i++)
	{
		//Init
		SetAnimIndex(static_cast<unsigned int>(i));
		float deltaTime = 0.0f;

		//Pre-Calculate All Possible Animations
		for (float ticks = 0; ticks < Animations[i].Duration; ticks += Animations[i].TicksPerSecond / 30.0f)
		{
			deltaTime += timeStep;
			Calculate(deltaTime);
			Animations[i].Transforms.push_back(vector<XMFLOAT4X4>());
			vector<XMFLOAT4X4>& currTransMatrix = Animations[i].Transforms.back();
			for (size_t a = 0; a < Transforms.size(); ++a)
			{
				XMMATRIX result = XMMatrixMultiply(XMLoadFloat4x4(&Bones[a]->Offset), XMLoadFloat4x4(&Bones[a]->GlobalTransform));
				XMFLOAT4X4 element;
				XMStoreFloat4x4(&element, result);
				currTransMatrix.push_back(element);
			}
		}
	}
}



void SceneAnimator::ExtractAnimations(const aiScene* pScene)
{
	//Add Animation
	for (size_t i(0); i < pScene->mNumAnimations; i++)
	{
		Animations.push_back(AnimationEvaluator(pScene->mAnimations[i]));
	}

	//Build Map for all Model's Animations
	for (uint32_t i(0); i < Animations.size(); i++)
	{
		AnimationNameToId.insert(map<string, uint32_t>::value_type(Animations[i].Name, i));
	}
	CurrentAnimIndex = 0;
}


bool SceneAnimator::SetAnimation(const string& name)
{
	//Init
	map<string, uint32_t>::iterator animationIterator = AnimationNameToId.find(name);
	int32_t prevAnimIndex = CurrentAnimIndex;

	//Set Proper Animation
	if (animationIterator != AnimationNameToId.end())
	{
		CurrentAnimIndex = animationIterator->second;
	}

	//New Animation Index Validation
	return prevAnimIndex != CurrentAnimIndex;
}


bool SceneAnimator::SetAnimIndex(int32_t indexValue)
{
	//Invalid Animation Index Check
	if (static_cast<size_t>(indexValue) >= Animations.size())
	{
		return false;
	}

	//Set New Animation Index
	int32_t prevAnimIndex = CurrentAnimIndex;
	CurrentAnimIndex = indexValue;

	//New Animation Index Validation
	return prevAnimIndex != CurrentAnimIndex;
}


void SceneAnimator::Calculate(float pTime)
{
	//Invalid Animation Index Check
	if ((CurrentAnimIndex < 0) || (static_cast<size_t>(CurrentAnimIndex) >= Animations.size()))
	{
		return;
	}

	Animations[CurrentAnimIndex].Evaluate(pTime, BonesByName);
	UpdateTransforms(Skeleton);
}



Bone* SceneAnimator::CreateBoneTree(aiNode* pNode, Bone* pParent)
{
	//Init
	Bone* internalNode = new Bone();
	internalNode->Name = pNode->mName.data;
	internalNode->Parent = pParent;

	//Index By Name
	BonesByName[internalNode->Name] = internalNode;

	//Extract Local Matrix from ASSIMP
	TransformMatrix(internalNode->LocalTransform, pNode->mTransformation);
	XMStoreFloat4x4(&internalNode->LocalTransform, XMMatrixTranspose(XMLoadFloat4x4(&internalNode->LocalTransform)));

	//Save A Copy
	internalNode->OriginalLocalTransform = internalNode->LocalTransform;

	//Calculate World Matrix
	CalculateBoneToWorldTransform(internalNode);

	//Recursively Build the Bone Tree
	for (unsigned int a = 0; a < pNode->mNumChildren; a++)
	{
		internalNode->Children.push_back(CreateBoneTree(pNode->mChildren[a], internalNode));
	}
	return internalNode;
}


void SceneAnimator::UpdateTransforms(Bone* pNode)
{
	//Update Root Global Matrix
	CalculateBoneToWorldTransform(pNode);

	//Update Children's Too
	for (auto aBone : pNode->Children)
	{
		UpdateTransforms(aBone);
	}
}


void SceneAnimator::CalculateBoneToWorldTransform(Bone* child)
{
	//Init
	child->GlobalTransform = child->LocalTransform;
	Bone* parent = child->Parent;

	//Traverse All Bone Nodes, while Concatenating their Matrices
	while (parent)
	{
		//Process This Node
		XMMATRIX concatenatedMatrix = XMMatrixMultiply(XMLoadFloat4x4(&child->GlobalTransform), XMLoadFloat4x4(&parent->LocalTransform));
		XMStoreFloat4x4(&child->GlobalTransform, concatenatedMatrix);

		//Continue On Up The Hierarchy
		parent = parent->Parent;
	}
}