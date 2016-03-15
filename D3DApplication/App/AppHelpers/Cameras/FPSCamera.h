#pragma once
#include "../Cameras/BaseCamera.h"

class Keyboard;
class Mouse;

class FPSCamera : public BaseCamera
{
	RTTI_DECLARATIONS(FPSCamera, BaseCamera)

public:
	FPSCamera(BaseApp& game);
	FPSCamera(BaseApp& game, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);
	virtual ~FPSCamera();

	const Keyboard& GetKeyboard() const;
	void SetKeyboard(Keyboard& keyboard);
	const Mouse& GetMouse() const;
	void SetMouse(Mouse& mouse);
	float& GetMouseSensitivity();
	float& GetRotationRate();
	float& GetMovementRate();

	virtual void Initialize() override;
	virtual void Update(const Time& deltaTime) override;

	static const float DefaultMouseSensitivity;
	static const float DefaultRotationRate;
	static const float DefaultMovementRate;

protected:
	float mMouseSensitivity;
	float mRotationRate;
	float mMovementRate;

	Keyboard* mKeyboard;
	Mouse* mMouse;

private:
	FPSCamera(const FPSCamera& rhs);
	FPSCamera& operator=(const FPSCamera& rhs) = delete;
};