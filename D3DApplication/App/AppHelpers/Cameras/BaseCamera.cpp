#include "BaseCamera.h"
#include "../../BaseApp.h"
#include "../Misc/Time.h"
#include "../Math/VectorDefines.h"
#include "../Math/MatrixFuncs.h"

RTTI_DEFINITIONS(BaseCamera)

const float BaseCamera::DefaultFieldOfView = XM_PIDIV4;
const float BaseCamera::DefaultNearZPlaneDistance = 0.01f;
const float BaseCamera::DefaultFarZPlaneDistance = 1000.0f;


BaseCamera::BaseCamera(BaseApp& baseApp) : BaseComponent(baseApp),
	mFieldOfView(DefaultFieldOfView), mAspectRatio(baseApp.GetAspectRatio()), mNearZPlaneDistance(DefaultNearZPlaneDistance),
	mFarZPlaneDistance(DefaultFarZPlaneDistance), mPosition(), mLook(), mUp(), mRight(), mViewMatrix(), mProjectionMatrix() { }

BaseCamera::BaseCamera(BaseApp& baseApp, float foV, float aspectRatio, float nearPlaneZDist, float farPlaneZDist) :	BaseComponent(baseApp),
	mFieldOfView(foV), mAspectRatio(baseApp.GetAspectRatio()), mNearZPlaneDistance(nearPlaneZDist), mFarZPlaneDistance(farPlaneZDist),
	mPosition(), mLook(), mUp(), mRight(), mViewMatrix(), mProjectionMatrix() { }

BaseCamera::~BaseCamera() { }

const XMFLOAT3& BaseCamera::GetPosition() const
{
	return mPosition;
}

const XMFLOAT3& BaseCamera::GetLook() const
{
	return mLook;
}

const XMFLOAT3& BaseCamera::GetUp() const
{
	return mUp;
}

const XMFLOAT3& BaseCamera::GetRight() const
{
	return mRight;
}

XMVECTOR BaseCamera::GetPositionXM() const
{
	return XMLoadFloat3(&mPosition);
}

XMVECTOR BaseCamera::GetLookXM() const
{
	return XMLoadFloat3(&mLook);
}

XMVECTOR BaseCamera::GetUpXM() const
{
	return XMLoadFloat3(&mUp);
}

XMVECTOR BaseCamera::GetRightXM() const
{
	return XMLoadFloat3(&mPosition);
}

float BaseCamera::GetAspectRatio() const
{
	return mAspectRatio;
}

float BaseCamera::GetFieldOfView() const
{
	return mFieldOfView;
}

float BaseCamera::GetNearPlaneDistance() const
{
	return mNearZPlaneDistance;
}

float BaseCamera::GetFarPlaneDistance() const
{
	return mFarZPlaneDistance;
}

XMMATRIX BaseCamera::GetViewMatrix() const
{
	return XMLoadFloat4x4(&mViewMatrix);
}

XMMATRIX BaseCamera::GetProjMatrix() const
{
	return XMLoadFloat4x4(&mProjectionMatrix);
}

XMMATRIX BaseCamera::GetViewProjMatrix() const
{
	return XMMatrixMultiply(XMLoadFloat4x4(&mViewMatrix), XMLoadFloat4x4(&mProjectionMatrix));
}

void BaseCamera::SetPosition(float x, float y, float z)
{
	SetPosition(XMVectorSet(x, y, z, 1.0f));
}

void BaseCamera::SetPosition(FXMVECTOR position)
{
	XMStoreFloat3(&mPosition, position);
}

void BaseCamera::SetPosition(const XMFLOAT3& position)
{
	mPosition = position;
}

void BaseCamera::Reset()
{
	mPosition = Vector3Helper::Zero;
	mLook = Vector3Helper::Forward;
	mRight = Vector3Helper::Right;
	mUp = Vector3Helper::Up;

	UpdateViewMatrix();
}

void BaseCamera::Initialize()
{
	UpdateProjectionMatrix();
	Reset();
}

void BaseCamera::Update(const Time& deltaTime)
{
	UpdateViewMatrix();
	
}

void BaseCamera::UpdateViewMatrix()
{
	XMVECTOR cameraPosition = XMLoadFloat3(&mPosition);
	XMVECTOR cameraLookVector = XMLoadFloat3(&mLook);
	XMVECTOR cameraUpVector = XMLoadFloat3(&mUp);

	XMMATRIX viewMatrix = XMMatrixLookToRH(cameraPosition, cameraLookVector, cameraUpVector);
	XMStoreFloat4x4(&mViewMatrix, viewMatrix);
}

void BaseCamera::UpdateProjectionMatrix()
{
	XMMATRIX projMatrix = XMMatrixPerspectiveFovRH(mFieldOfView, mAspectRatio, mNearZPlaneDistance, mFarZPlaneDistance);
	XMStoreFloat4x4(&mProjectionMatrix, projMatrix);
}

void BaseCamera::ApplyRotation(CXMMATRIX transform)
{
	XMVECTOR cameraLookVector = XMLoadFloat3(&mLook);
	XMVECTOR cameraUpVector = XMLoadFloat3(&mUp);

	cameraLookVector = XMVector3TransformNormal(cameraLookVector, transform);
	cameraUpVector = XMVector3TransformNormal(cameraUpVector, transform);

	cameraLookVector = XMVector3Normalize(cameraLookVector);
	cameraUpVector = XMVector3Normalize(cameraUpVector);

	XMVECTOR cameraRightVector = XMVector3Cross(cameraLookVector, cameraUpVector);
	cameraUpVector = XMVector3Cross(cameraRightVector, cameraLookVector);

	XMStoreFloat3(&mLook, cameraLookVector);
	XMStoreFloat3(&mUp, cameraUpVector);
	XMStoreFloat3(&mRight, cameraRightVector);
}

void BaseCamera::ApplyRotation(const XMFLOAT4X4& transform)
{
	ApplyRotation(XMLoadFloat4x4(&transform));
}