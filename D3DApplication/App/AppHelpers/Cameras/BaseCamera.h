#pragma once
#include "../Components/BaseComponent.h"

class Time;

class BaseCamera : public BaseComponent
{
	RTTI_DECLARATIONS(BaseCamera, BaseComponent)
public:
	BaseCamera(BaseApp& baseApp);
	BaseCamera(BaseApp& baseApp, float foV, float aspectRatio, float nearPlaneZDist, float farPlaneZDist);
	virtual ~BaseCamera();

	const XMFLOAT3& GetPosition() const;
	const XMFLOAT3& GetLook() const;
	const XMFLOAT3& GetUp() const;
	const XMFLOAT3& GetRight() const;

	XMVECTOR GetPositionXM() const;
	XMVECTOR GetLookXM() const;
	XMVECTOR GetUpXM() const;
	XMVECTOR GetRightXM() const;

	float GetAspectRatio() const;
	float GetFieldOfView() const;
	float GetNearPlaneDistance() const;
	float GetFarPlaneDistance() const;

	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjMatrix() const;
	XMMATRIX GetViewProjMatrix() const;

	virtual void SetPosition(float x, float y, float z);
	virtual void SetPosition(FXMVECTOR position);
	virtual void SetPosition(const XMFLOAT3& position);

	virtual void Reset();
	virtual void Initialize() override;
	virtual void Update(const Time& deltaTime) override;
	virtual void UpdateViewMatrix();
	virtual void UpdateProjectionMatrix();

	void ApplyRotation(CXMMATRIX transform);
	void ApplyRotation(const XMFLOAT4X4& transform);

	static const float DefaultFieldOfView;
	static const float DefaultAspectRatio;
	static const float DefaultNearZPlaneDistance;
	static const float DefaultFarZPlaneDistance;

protected:
	float mFieldOfView;
	float mAspectRatio;
	float mNearZPlaneDistance;
	float mFarZPlaneDistance;

	XMFLOAT3 mPosition;
	XMFLOAT3 mLook;
	XMFLOAT3 mUp;
	XMFLOAT3 mRight;

	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mProjectionMatrix;

private:
	//No Copy
	BaseCamera(const BaseCamera& rhs) = delete;
	BaseCamera& operator=(const BaseCamera& rhs) = delete;
};