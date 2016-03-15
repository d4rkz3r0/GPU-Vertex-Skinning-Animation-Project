#include "SceneNode.h"
#include "../../../App/AppHelpers/Math/MatrixFuncs.h"
#include "../../../Shared/SharedUtils.h"

RTTI_DEFINITIONS(SceneNode)

SceneNode::SceneNode(const string& name) :
	mName(name), mParent(nullptr), mChildren(), mTransform(MatrixFuncs::Identity)
{ }

SceneNode::SceneNode(const string& name, const XMFLOAT4X4& transform)
	: mName(name), mParent(nullptr), mChildren(), mTransform(transform)
{ }

const string& SceneNode::Name() const
{
	return mName;
}

SceneNode* SceneNode::GetParent()
{
	return mParent;
}

vector<SceneNode*>& SceneNode::Children()
{
	return mChildren;
}

const XMFLOAT4X4& SceneNode::Transform() const
{
	return mTransform;
}

XMMATRIX SceneNode::TransformMatrix() const
{
	return XMLoadFloat4x4(&mTransform);
}

void SceneNode::SetParent(SceneNode* parent)
{
	mParent = parent;
}

void SceneNode::SetTransform(XMFLOAT4X4& transform)
{
	mTransform = transform;
}

void SceneNode::SetTransform(CXMMATRIX transform)
{
	XMFLOAT4X4 t;
	XMStoreFloat4x4(&t, transform);

	SetTransform(t);
}