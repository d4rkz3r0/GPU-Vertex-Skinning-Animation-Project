#include "BaseComponent.h"
#include "../Misc/Time.h"

RTTI_DEFINITIONS(BaseComponent)

BaseComponent::BaseComponent() : mBaseApp(nullptr), mEnabled(true) {}

BaseComponent::BaseComponent(BaseApp& baseApp) : mBaseApp(&baseApp), mEnabled(true) {}

BaseComponent::~BaseComponent() {}

BaseApp* BaseComponent::GetApp()
{
	return mBaseApp;
}

void BaseComponent::SetApp(BaseApp& baseApp)
{
	mBaseApp = &baseApp;
}

bool BaseComponent::IsEnabled() const
{
	return mEnabled;
}

void BaseComponent::SetEnabled(bool enabled)
{
	mEnabled = enabled;
}

void BaseComponent::Initialize() {}

void BaseComponent::Update(const Time& deltaTime) {}
