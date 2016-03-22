#include "BaseMesh.h"
#include "../../../AppHelpers/Cameras/Camera.h"
#include "../../../Shared/SharedUtils.h"


BaseMesh::BaseMesh(Camera& camera) : mCamera(&camera)
{
	mEnabled = true;
	mFileName = "";
	mFileType = "";
	mFilePath = "";
	mTextureFilePath = "Assets\\Textures\\";
	mDiffuseMapFileName = "";
	mNormalMapFileName = "";
	XMStoreFloat4x4(&mTranslationMX, XMMatrixIdentity());
	XMStoreFloat4x4(&mScalingMX, XMMatrixScaling(0.075f, 0.75f, 0.75f));
	XMStoreFloat4x4(&mRotationMX, XMMatrixIdentity());
}

BaseMesh::~BaseMesh()
{
	BaseMesh::Shutdown();
}

Camera& BaseMesh::GetCamera()
{
	return *mCamera;
}

bool BaseMesh::IsEnabled() const
{
	return mEnabled;
}

void BaseMesh::SetEnabled(bool enabled)
{
	mEnabled = enabled;
}

void BaseMesh::SetWorld(CXMMATRIX matrixVal)
{
	mWorldMX = matrixVal;
}

void BaseMesh::Shutdown()
{
	
}

XMMATRIX BaseMesh::GetWorld()
{
	mWorldMX = XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&mScalingMX), XMLoadFloat4x4(&mRotationMX)), XMLoadFloat4x4(&mTranslationMX));
	return mWorldMX;
}

void BaseMesh::SetScaling(CXMMATRIX matrixVal)
{
	XMStoreFloat4x4(&mScalingMX, matrixVal);
}

void BaseMesh::SetRotation(CXMMATRIX matrixVal)
{
	XMStoreFloat4x4(&mRotationMX, matrixVal);
}

void BaseMesh::SetTranslation(CXMMATRIX matrixVal)
{
	XMStoreFloat4x4(&mTranslationMX, matrixVal);
}