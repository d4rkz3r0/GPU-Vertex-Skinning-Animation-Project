#pragma once
#include "../Components/BaseComponent.h"
#include "../../BaseApp.h"
#include <DirectXMath.h>
using namespace DirectX;

class Time;
class BaseApp;
class Keyboard;
class Mouse;

class Camera : public BaseComponent
{
	RTTI_DECLARATIONS(Camera, BaseComponent)

public:
	Camera(BaseApp& baseApp);
	~Camera();

	virtual void Initialize() override;
	void Update(Time& deltaTime) override;

	//Accessors & Mutators
	XMFLOAT3 GetRight() const;
	XMFLOAT3 GetUp() const;
	XMFLOAT3 GetLook() const;
	XMFLOAT3 GetPosition() const;

	void SetRight(float x, float y, float z);
	void SetUp(float x, float y, float z);
	void SetLook(float x, float y, float z);
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& position);

	void ApplyRotation(CXMMATRIX transform);
	void ApplyRotation(const XMFLOAT4X4& transform);

	float& GetMouseSensitivity();
	float& GetMovementRate();

	//Direct Input
	const Keyboard& GetKeyboard() const;
	void SetKeyboard(Keyboard& keyboard);
	const Mouse& GetMouse() const;
	void SetMouse(Mouse& mouse);

	//SIMD Accessors
	XMVECTOR GetUpXM() const;
	XMVECTOR GetRightXM() const;
	XMVECTOR GetLookXM() const;
	XMVECTOR GetPositionXM() const;
	XMMATRIX GetView() const;
	XMMATRIX GetProj() const;
	XMMATRIX GetViewProj() const;


	//Camera & Frustum
	void SetFrustum(float foVY, float aspectRatio, float nearZ, float farZ);

	//View Matrix Helpers (Optional)
	void PrepView(XMFLOAT3& position, XMFLOAT3& target, XMFLOAT3& up);
	void SetView(XMVECTOR _vPosition, XMVECTOR _vTarget, XMVECTOR _vUp);

	//Camera Transformation
	void VFly(float moveSpeed);
	void Strafe(float moveSpeed);
	void Step(float moveSpeed);
	void Pitch(float angle);
	void RotateY(float angle);

	void UpdateViewMatrix();

private:
	//No Copy
	Camera(const Camera& rhs) = delete;
	Camera& operator=(const Camera& rhs) = delete;

	//Camera's Local Coordinate System
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	XMFLOAT3 mLook;
	XMFLOAT3 mPosition;

	//Frustum's Properties
	float mFoV;
	float mAspectRatio;
	float mNearZ;
	float mFarZ;

	//Camera and it's Frustum
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	Keyboard* mKeyboard;
	Mouse* mMouse;

	float mMouseSensitivity = 10.0f;
	float mCameraMovementRate = 75.0f;
};