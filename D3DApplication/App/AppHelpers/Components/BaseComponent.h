#pragma once
#include "../../../Shared/RTTI.h"
#include "../../../Shared/SharedUtils.h"

//Forward Declarations
class BaseApp;
class Time;

//Component Based System
class BaseComponent : public RTTI
{
	RTTI_DECLARATIONS(BaseComponent, RTTI)
public:
	BaseComponent();
	BaseComponent(BaseApp& baseApp);
	virtual ~BaseComponent();

	BaseApp* GetApp();
	virtual void SetApp(BaseApp& baseApp);
	virtual bool IsEnabled() const;
	virtual void SetEnabled(bool enabled);

	virtual void Initialize();
	virtual void Update(Time& deltaTime);

protected:
	BaseApp* mBaseApp;
	bool mEnabled;

private:
	BaseComponent(const BaseComponent& rhs);
	BaseComponent& operator=(const BaseComponent& rhs);
};