#include "RenderableComponent.h"

RTTI_DEFINITIONS(RenderableComponent)

RenderableComponent::RenderableComponent() : BaseComponent(), mVisible(true), mCamera(nullptr) {}

RenderableComponent::RenderableComponent(BaseApp& baseApp) : BaseComponent(baseApp), mVisible(true), mCamera(nullptr) {}

RenderableComponent::RenderableComponent(BaseApp& baseApp, Camera& camera) : BaseComponent(baseApp), mVisible(true), mCamera(&camera) {}

RenderableComponent::~RenderableComponent() {}

bool RenderableComponent::IsVisible() const
{
	return mVisible;
}

void RenderableComponent::SetVisible(bool visible)
{
	mVisible = visible;
}

Camera* RenderableComponent::GetCamera()
{
	return mCamera;
}

void RenderableComponent::SetCamera(Camera* camera)
{
	mCamera = camera;
}

void RenderableComponent::Draw(const Time& deltaTime) {}
