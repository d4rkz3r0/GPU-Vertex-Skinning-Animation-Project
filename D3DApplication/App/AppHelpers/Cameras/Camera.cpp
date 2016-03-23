#include "Camera.h"
#include "../../BaseApp.h"
#include "../Misc/Time.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Math/VectorDefines.h"

RTTI_DEFINITIONS(Camera)

Camera::Camera(BaseApp& baseApp) : BaseComponent(baseApp),
mRight(1.0f, 0.0f, 0.0f), mUp(0.0f, 1.0f, 0.0f), mLook(0.0f, 0.0f, 1.0f), mPosition(0.0f, 0.0f, 0.0f),
mFoV(XM_PIDIV4), mAspectRatio(baseApp.GetAspectRatio()), mNearZ(0.5f), mFarZ(1000.f),
mView(), mProj()
{
	SetFrustum(mFoV, mAspectRatio, mNearZ, mFarZ);
}

Camera::~Camera()
{
	mKeyboard = nullptr;
	mMouse = nullptr;
}


const Keyboard& Camera::GetKeyboard() const
{
	return *mKeyboard;
}

void Camera::SetKeyboard(Keyboard& keyboard)
{
	mKeyboard = &keyboard;
}

const Mouse& Camera::GetMouse() const
{
	return *mMouse;
}

void Camera::SetMouse(Mouse& mouse)
{
	mMouse = &mouse;
}

void Camera::Initialize()
{
	mKeyboard = static_cast<Keyboard*>(mBaseApp->GetServices().GetService(Keyboard::TypeIdClass()));
	mMouse = static_cast<Mouse*>(mBaseApp->GetServices().GetService(Mouse::TypeIdClass()));
}

void Camera::Update(const Time& deltaTime)
{
	if (mKeyboard != nullptr)
	{
		if (mKeyboard->IsKeyDown(DIK_W))
		{
			Step(mMovementRate * static_cast<float>(deltaTime.ElapsedTime()));
		}

		if (mKeyboard->IsKeyDown(DIK_S))
		{
			Step(-mMovementRate  * static_cast<float>(deltaTime.ElapsedTime()));
		}

		if (mKeyboard->IsKeyDown(DIK_A))
		{
			Strafe(-mMovementRate * static_cast<float>(deltaTime.ElapsedTime()));
		}

		if (mKeyboard->IsKeyDown(DIK_D))
		{
			Strafe(mMovementRate * static_cast<float>(deltaTime.ElapsedTime()));
		}

		if (mKeyboard->IsKeyDown(DIK_Q))
		{
			VFly(mMovementRate *static_cast<float>(deltaTime.ElapsedTime()));
		}
		if (mKeyboard->IsKeyDown(DIK_E))
		{
			VFly(-mMovementRate * static_cast<float>(deltaTime.ElapsedTime()));
		}
	}

	//if(mMouse != nullptr)
	//{

		//Win32
		//POINT mPrevMousePos;

	/*	void OnMouseDown(WPARAM theButton, int x, int y)
		{
			mPrevMousePos.x = x;
			mPrevMousePos.y = y;

			SetCapture(mMainWnd);
		}

		//Mouse Input, using Windowsx.h Macros and fallthrough
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		{
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
		}
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		{
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
		}
		case WM_MOUSEMOVE:
		{
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
		}

		void OnMouseUp(WPARAM theButton, int x, int y)
		{
			ReleaseCapture();
		}

		void OnMouseMove(WPARAM theButton, int x, int y)
		{
			if (theButton & MK_LBUTTON)
			{
				float currYRotAngle = XMConvertToRadians(-0.25f * static_cast<float>(x - mPrevMousePos.x));
				float currXRotAngle = XMConvertToRadians(-0.25f * static_cast<float>(y - mPrevMousePos.y));
				mCamera.Pitch(-currXRotAngle);
				mCamera.RotateY(-currYRotAngle);
			}
			mPrevMousePos.x = x;
			mPrevMousePos.y = y;
		}*/

	/*	if (mMouse->Is(DIK_W))
		{
			Step(mMovementRate * static_cast<float>(deltaTime.ElapsedTime()));
		}

		if (mKeyboard->IsKeyDown(DIK_S))
		{
			Step(-mMovementRate  * static_cast<float>(deltaTime.ElapsedTime()));
		}

		if (mKeyboard->IsKeyDown(DIK_A))
		{
			Strafe(-mMovementRate * static_cast<float>(deltaTime.ElapsedTime()));
		}

		if (mKeyboard->IsKeyDown(DIK_D))
		{
			Strafe(mMovementRate * static_cast<float>(deltaTime.ElapsedTime()));
		}

		if (mKeyboard->IsKeyDown(DIK_Q))
		{
			VFly(mMovementRate *static_cast<float>(deltaTime.ElapsedTime()));
		}
		if (mKeyboard->IsKeyDown(DIK_E))
		{
			VFly(-mMovementRate * static_cast<float>(deltaTime.ElapsedTime()));
		}
	}*/

	XMFLOAT2 rotationAmount = Vector2Helper::Zero;
	if ((mMouse != nullptr) && (mMouse->IsButtonHeldDown(MouseButtonLeft)))
	{
		LPDIMOUSESTATE mouseState = mMouse->GetCurrentState();
		rotationAmount.x = -mouseState->lX * mMouseSensitivity;
		rotationAmount.y = -mouseState->lY * mMouseSensitivity;
	}

	float elapsedTime = static_cast<float>(deltaTime.ElapsedTime());
	XMVECTOR rotationVector = XMLoadFloat2(&rotationAmount) * mRotationRate * elapsedTime;
	XMVECTOR right = XMLoadFloat3(&mRight);

	XMMATRIX pitchMatrix = XMMatrixRotationAxis(right, XMVectorGetY(rotationVector));
	XMMATRIX yawMatrix = XMMatrixRotationY(XMVectorGetX(rotationVector));

	//ApplyRotation(XMMatrixMultiply(pitchMatrix, yawMatrix));

	UpdateViewMatrix();
}

XMFLOAT3 Camera::GetRight() const
{
	return mRight;
}

XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetUp() const
{
	return mUp;
}

XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetLook() const
{
	return mLook;
}

XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetPosition() const
{
	return mPosition;
}

XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&mPosition);
}

XMMATRIX Camera::GetView() const
{
	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::GetProj() const
{
	return XMLoadFloat4x4(&mProj);
}

XMMATRIX Camera::GetViewProj() const
{
	return (XMMatrixMultiply(GetView(), GetProj()));
}

float& Camera::GetMouseSensitivity()
{
	return mMouseSensitivity;
}

float& Camera::GetRotationRate()
{
	return mRotationRate;
}

float& Camera::GetMovementRate()
{
	return mMovementRate;
}

void Camera::SetRight(float x, float y, float z)
{
	mRight = XMFLOAT3(x, y, z);
}

void Camera::SetUp(float x, float y, float z)
{
	mUp = XMFLOAT3(x, y, z);
}

void Camera::SetLook(float x, float y, float z)
{
	mLook = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3& position)
{
	mPosition = position;
}

void Camera::SetFrustum(float foVY, float aspectRatio, float nearZ, float farZ)
{
	mFoV = foVY;
	mAspectRatio = aspectRatio;
	mNearZ = nearZ;
	mFarZ = farZ;

	XMStoreFloat4x4(&mProj, XMMatrixPerspectiveFovLH(mFoV, mAspectRatio, mNearZ, mFarZ));
}

void Camera::PrepView(XMFLOAT3& position, XMFLOAT3& target, XMFLOAT3& up)
{
	XMVECTOR vPosition = XMLoadFloat3(&position);
	XMVECTOR vTarget = XMLoadFloat3(&target);
	XMVECTOR vUp = XMLoadFloat3(&up);

	SetView(vPosition, vTarget, vUp);
}

void Camera::SetView(XMVECTOR _vPosition, XMVECTOR _vTarget, XMVECTOR _vUp)
{
	XMVECTOR vLook = XMVector3Normalize(XMVectorSubtract(_vTarget, _vPosition));
	XMVECTOR vRight = XMVector3Normalize(XMVector3Cross(_vUp, vLook));
	XMVECTOR vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

	XMStoreFloat3(&mPosition, _vPosition);
	XMStoreFloat3(&mLook, vLook);
	XMStoreFloat3(&mRight, vRight);
	XMStoreFloat3(&mUp, vUp);
}

void Camera::Strafe(float moveSpeed)
{
	XMVECTOR vMoveSpeed = XMVectorReplicate(moveSpeed);
	XMVECTOR vPosition = XMLoadFloat3(&mPosition);
	XMVECTOR vRight = XMLoadFloat3(&mRight);

	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(vMoveSpeed, vRight, vPosition));
}

void Camera::Step(float moveSpeed)
{
	XMVECTOR vMoveSpeed = XMVectorReplicate(moveSpeed);
	XMVECTOR vPosition = XMLoadFloat3(&mPosition);
	XMVECTOR vLook = XMLoadFloat3(&mLook);

	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(vMoveSpeed, vLook, vPosition));
}

void Camera::VFly(float moveSpeed)
{
	XMVECTOR vMoveSpeed = XMVectorReplicate(moveSpeed);
	XMVECTOR vGlobalY = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR vPosition = XMLoadFloat3(&mPosition);

	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(vMoveSpeed, vGlobalY, vPosition));
}

void Camera::Pitch(float angle)
{
	XMMATRIX camXRotationMX = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);
	
	//Grab camera Y and Z axes
	XMVECTOR vUp = XMLoadFloat3(&mUp);
	XMVECTOR vLook = XMLoadFloat3(&mLook);

	//Don't use RightRotationMX's position vector when multiplying
	XMVECTOR vUpdatedUp = XMVector3TransformNormal(vUp, camXRotationMX);
	XMVECTOR vUpdatedLook = XMVector3TransformNormal(vLook, camXRotationMX);

	XMStoreFloat3(&mUp, vUpdatedUp);
	XMStoreFloat3(&mLook, vUpdatedLook);
}

void Camera::RotateY(float angle)
{
	XMMATRIX globalYRotationMX = XMMatrixRotationY(angle);

	//Grab all camera axes
	XMVECTOR vRight = XMLoadFloat3(&mRight);
	XMVECTOR vLook = XMLoadFloat3(&mLook);
	XMVECTOR vUp = XMLoadFloat3(&mUp);

	//Don't use YRotationMX's position vector when multiplying
	XMVECTOR vUpdatedRight = XMVector3TransformNormal(vRight, globalYRotationMX);
	XMVECTOR vUpdatedUp = XMVector3TransformNormal(vUp, globalYRotationMX);
	XMVECTOR vUpdatedLook = XMVector3TransformNormal(vLook, globalYRotationMX);

	XMStoreFloat3(&mRight, vUpdatedRight);
	XMStoreFloat3(&mLook, vUpdatedLook);
	XMStoreFloat3(&mUp, vUpdatedUp);
}

void Camera::UpdateViewMatrix()
{
	//Load
	XMVECTOR vRight = XMLoadFloat3(&mRight);
	XMVECTOR vUp = XMLoadFloat3(&mUp);
	XMVECTOR vLook = XMLoadFloat3(&mLook);
	XMVECTOR vPosition = XMLoadFloat3(&mPosition);

	//(Camera's Orientation)
	//Re-Orthonormalize Basis Vectors
	vLook = XMVector3Normalize(vLook);
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	vRight = XMVector3Cross(vUp, vLook);
	
	//(Camera's Position)
	//newPos = currPos * normalizedDirectionVec
	float newXPos = -XMVectorGetX(XMVector3Dot(vPosition, vRight));
	float newYPos = -XMVectorGetX(XMVector3Dot(vPosition, vUp));
	float newZPos = -XMVectorGetX(XMVector3Dot(vPosition, vLook));

	//Save
	XMStoreFloat3(&mRight, vRight);
	XMStoreFloat3(&mUp, vUp);
	XMStoreFloat3(&mLook, vLook);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = newXPos;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = newYPos;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = newZPos;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;
}

void Camera::ApplyRotation(CXMMATRIX transform)
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

void Camera::ApplyRotation(const XMFLOAT4X4& transform)
{
	ApplyRotation(XMLoadFloat4x4(&transform));
}