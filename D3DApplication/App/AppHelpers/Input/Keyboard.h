#pragma once
#include "../Components/BaseComponent.h"

class Keyboard : public BaseComponent
{
	RTTI_DECLARATIONS(Keyboard, BaseComponent)

public:
	Keyboard(BaseApp& baseApp, LPDIRECTINPUT8 directInput);
	~Keyboard();

	const byte* CurrentState() const;
	const byte* LastState() const;

	virtual void Initialize() override;
	virtual void Update(Time& deltaTime) override;

	bool IsKeyUp(byte key) const;
	bool IsKeyDown(byte key) const;
	bool WasKeyUp(byte key) const;
	bool WasKeyDown(byte key) const;
	bool WasKeyPressedThisFrame(byte key) const;
	bool WasKeyReleasedThisFrame(byte key) const;
	bool IsKeyHeldDown(byte key) const;

private:
	Keyboard() = delete;
	Keyboard(const Keyboard& rhs);

	LPDIRECTINPUT8 mDirectInput;
	IDirectInputDevice8* mDirectInputDevice;
	static const int KeyCount = 256;
	byte mCurrentState[KeyCount];
	byte mLastState[KeyCount];
};