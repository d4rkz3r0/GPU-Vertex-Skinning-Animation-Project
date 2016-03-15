#include "FPSCamera.h"
#include "../../BaseApp.h"
#include "../Misc/Time.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Math/VectorDefines.h"

RTTI_DEFINITIONS(FPSCamera)

const float FPSCamera::DefaultRotationRate = XMConvertToRadians(1.0f);
const float FPSCamera::DefaultMovementRate = 15.0f;
const float FPSCamera::DefaultMouseSensitivity = 110.0f;

FPSCamera::FPSCamera(BaseApp& game)	: BaseCamera(game), mMouseSensitivity(DefaultMouseSensitivity), mRotationRate(DefaultRotationRate),
	mMovementRate(DefaultMovementRate), mKeyboard(nullptr), mMouse(nullptr) { }

FPSCamera::FPSCamera(BaseApp& game, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
	: BaseCamera(game, fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance), mMouseSensitivity(DefaultMouseSensitivity), mRotationRate(DefaultRotationRate),
	mMovementRate(DefaultMovementRate), mKeyboard(nullptr), mMouse(nullptr) { }

FPSCamera::~FPSCamera()
{
	mKeyboard = nullptr;
	mMouse = nullptr;
}

const Keyboard& FPSCamera::GetKeyboard() const
{
	return *mKeyboard;
}

void FPSCamera::SetKeyboard(Keyboard& keyboard)
{
	mKeyboard = &keyboard;
}

const Mouse& FPSCamera::GetMouse() const
{
	return *mMouse;
}

void FPSCamera::SetMouse(Mouse& mouse)
{
	mMouse = &mouse;
}

float&FPSCamera::GetMouseSensitivity()
{
	return mMouseSensitivity;
}


float& FPSCamera::GetRotationRate()
{
	return mRotationRate;
}

float& FPSCamera::GetMovementRate()
{
	return mMovementRate;
}

void FPSCamera::Initialize()
{
	mKeyboard = static_cast<Keyboard*>(mBaseApp->GetServices().GetService(Keyboard::TypeIdClass()));
	mMouse = static_cast<Mouse*>(mBaseApp->GetServices().GetService(Mouse::TypeIdClass()));

	BaseCamera::Initialize();
}

void FPSCamera::Update(const Time& deltaTime)
{
	XMFLOAT2 movementAmount = Vector2Helper::Zero;
	float flyAmount = 0.0f;

	if (mKeyboard != nullptr)
	{
		if (mKeyboard->IsKeyDown(DIK_W))
		{
			movementAmount.y = 1.0f;
		}

		if (mKeyboard->IsKeyDown(DIK_S))
		{
			movementAmount.y = -1.0f;
		}

		if (mKeyboard->IsKeyDown(DIK_A))
		{
			movementAmount.x = -1.0f;
		}

		if (mKeyboard->IsKeyDown(DIK_D))
		{
			movementAmount.x = 1.0f;
		}

		if (mKeyboard->IsKeyDown(DIK_Q))
		{
			flyAmount = -1.0f;
		}
		if (mKeyboard->IsKeyDown(DIK_E))
		{
			flyAmount = +1.0f;
		}
	}

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

	ApplyRotation(XMMatrixMultiply(pitchMatrix, yawMatrix));

	XMVECTOR position = XMLoadFloat3(&mPosition);
	XMVECTOR movement = XMLoadFloat2(&movementAmount) * mMovementRate * elapsedTime;


	XMVECTOR strafe = right * XMVectorGetX(movement);
	position += strafe;

	XMVECTOR forward = XMLoadFloat3(&mLook) * XMVectorGetY(movement);
	position += forward;

	XMVECTOR fly = XMLoadFloat3(&mUp) * flyAmount * mMovementRate * elapsedTime;
	position += fly;

	XMStoreFloat3(&mPosition, position);

	BaseCamera::Update(deltaTime);
}