#pragma once
#include "../../../../../Shared/SharedUtils.h"

class SceneNode : public RTTI
{
	RTTI_DECLARATIONS(SceneNode, RTTI)

public:
	const string& Name() const;
	SceneNode* GetParent();
	vector<SceneNode*>& Children();
	const XMFLOAT4X4& Transform() const;
	XMMATRIX TransformMatrix() const;

	void SetParent(SceneNode* parent);
	void SetTransform(XMFLOAT4X4& transform);
	void SetTransform(CXMMATRIX transform);

	SceneNode(const string& name);
	SceneNode(const string& name, const XMFLOAT4X4& transform);

protected:
	string mName;
	SceneNode* mParent;
	vector<SceneNode*> mChildren;
	XMFLOAT4X4 mTransform;

private:
	SceneNode();
	SceneNode(const SceneNode& rhs);
	SceneNode& operator=(const SceneNode& rhs);
};