#pragma once
#include "../Components/BaseComponent.h"

class Time;

enum MouseButtons
{
	MouseButtonLeft = 0,
	MouseButtonRight = 1,
	MouseButtonMiddle = 2,
	MouseButtonX1 = 3
};

class Mouse : public BaseComponent
{
	RTTI_DECLARATIONS(Mouse, BaseComponent)

public:
	Mouse(BaseApp& baseApp, LPDIRECTINPUT8 directInput);
	~Mouse();

	LPDIMOUSESTATE GetCurrentState();
	LPDIMOUSESTATE GetLastState();

	virtual void Initialize() override;
	virtual void Update(Time& deltaTime) override;

	long X() const;
	long Y() const;
	long Wheel() const;

	bool IsButtonUp(MouseButtons button) const;
	bool IsButtonDown(MouseButtons button) const;
	bool WasButtonUp(MouseButtons button) const;
	bool WasButtonDown(MouseButtons button) const;
	bool WasButtonPressedThisFrame(MouseButtons button) const;
	bool WasButtonReleasedThisFrame(MouseButtons button) const;
	bool IsButtonHeldDown(MouseButtons button) const;

private:
	Mouse() = default;
	Mouse(const Mouse& rhs);

	LPDIRECTINPUT8 mDirectInput;
	LPDIRECTINPUTDEVICE8 mDirectInputDevice;
	DIMOUSESTATE mCurrentState;
	DIMOUSESTATE mLastState;

	long mX;
	long mY;
	long mWheel;
};
