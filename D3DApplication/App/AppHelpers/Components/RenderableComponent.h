#pragma once
#include "BaseComponent.h"

class Camera;

class RenderableComponent : public BaseComponent
{
	RTTI_DECLARATIONS(RenderableComponent, BaseComponent)

public:
	RenderableComponent();
	RenderableComponent(BaseApp& baseApp);
	RenderableComponent(BaseApp& baseApp, Camera& camera);
	virtual ~RenderableComponent();

	bool IsVisible() const;
	void SetVisible(bool visible);
	Camera* GetCamera();
	void SetCamera(Camera* camera);

	//Extension
	virtual void Draw(const Time& deltaTime);

protected:
	bool mVisible;
	Camera* mCamera;

private:
	RenderableComponent(const RenderableComponent& rhs) = delete;
	RenderableComponent& operator=(const RenderableComponent& rhs) = delete;
};