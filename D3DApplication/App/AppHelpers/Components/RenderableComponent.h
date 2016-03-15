#pragma once
#include "BaseComponent.h"

class BaseCamera;

class RenderableComponent : public BaseComponent
{
	RTTI_DECLARATIONS(RenderableComponent, BaseComponent)

public:
	RenderableComponent();
	RenderableComponent(BaseApp& baseApp);
	RenderableComponent(BaseApp& baseApp, BaseCamera& camera);
	virtual ~RenderableComponent();

	bool IsVisible() const;
	void SetVisible(bool visible);
	BaseCamera* GetCamera();
	void SetCamera(BaseCamera* camera);

	//Extension
	virtual void Draw(const Time& deltaTime);

protected:
	bool mVisible;
	BaseCamera* mCamera;

private:
	RenderableComponent(const RenderableComponent& rhs) = delete;
	RenderableComponent& operator=(const RenderableComponent& rhs) = delete;
};