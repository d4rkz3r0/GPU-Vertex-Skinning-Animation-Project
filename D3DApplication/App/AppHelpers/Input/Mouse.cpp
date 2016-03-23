#include "Mouse.h"
#include "../../BaseApp.h"
#include "../Misc/Time.h"
#include "../Misc/Exception.h"

RTTI_DEFINITIONS(Mouse)

Mouse::Mouse(BaseApp& baseApp, LPDIRECTINPUT8 directInput) : BaseComponent(baseApp), mDirectInput(directInput), mDirectInputDevice(nullptr), mX(0), mY(0), mWheel(0)
{
	assert(directInput != nullptr);
	ZeroMem(mCurrentState);
	ZeroMem(mLastState);
}

Mouse::~Mouse()
{
	if (mDirectInputDevice != nullptr)
	{
		mDirectInputDevice->Unacquire();
		ReleaseObject(mDirectInputDevice);
	}
}

LPDIMOUSESTATE Mouse::GetCurrentState()
{
	return &mCurrentState;
}

LPDIMOUSESTATE Mouse::GetLastState()
{
	return &mLastState;
}

long Mouse::X() const
{
	return mX;
}

long Mouse::Y() const
{
	return mY;
}

long Mouse::Wheel() const
{
	return mWheel;
}

void Mouse::Initialize()
{
	if (FAILED(mDirectInput->CreateDevice(GUID_SysMouse, &mDirectInputDevice, nullptr)))
	{
		throw Exception("Mouse::Initialize() failed.");
	}
	if (FAILED(mDirectInputDevice->SetDataFormat(&c_dfDIMouse)))
	{
		throw Exception("Mouse::SetDataFormat() failed.");
	}
	if (FAILED(mDirectInputDevice->SetCooperativeLevel(mBaseApp->GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		throw Exception("Mouse::SetCooperativeLevel() failed.");
	}
	mDirectInputDevice->Acquire();
}

void Mouse::Update(Time& deltaTime)
{
	if (mDirectInputDevice != nullptr)
	{
		memcpy(&mLastState, &mCurrentState, sizeof(mCurrentState));
		if (FAILED(mDirectInputDevice->GetDeviceState(sizeof(mCurrentState), &mCurrentState)))
		{
			if (SUCCEEDED(mDirectInputDevice->Acquire()))
			{
				if (FAILED(mDirectInputDevice->GetDeviceState(sizeof(mCurrentState), &mCurrentState)))
				{
					throw Exception("Mouse::Update(), unable to reaquire device or update.");
				}
			}
		}
	}

	mX += mCurrentState.lX;
	mY += mCurrentState.lY;
	mWheel += mCurrentState.lZ;
}

bool Mouse::IsButtonUp(MouseButtons button) const
{
	return ((mCurrentState.rgbButtons[button] & 0x80) == 0);
}

bool Mouse::IsButtonDown(MouseButtons button) const
{
	return ((mCurrentState.rgbButtons[button] & 0x80) != 0);
}

bool Mouse::WasButtonUp(MouseButtons button) const
{
	return ((mLastState.rgbButtons[button] & 0x80) == 0);
}

bool Mouse::WasButtonDown(MouseButtons button) const
{
	return ((mLastState.rgbButtons[button] & 0x80) != 0);
}

bool Mouse::WasButtonPressedThisFrame(MouseButtons button) const
{
	return (IsButtonDown(button) && WasButtonUp(button));
}

bool Mouse::WasButtonReleasedThisFrame(MouseButtons button) const
{
	return (IsButtonUp(button) && WasButtonDown(button));
}

bool Mouse::IsButtonHeldDown(MouseButtons button) const
{
	return (IsButtonDown(button) && WasButtonDown(button));
}