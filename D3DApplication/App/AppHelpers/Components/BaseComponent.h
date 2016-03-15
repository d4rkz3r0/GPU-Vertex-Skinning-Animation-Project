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
	void SetApp(BaseApp& baseApp);
	bool IsEnabled() const;
	void SetEnabled(bool enabled);

	virtual void Initialize();
	virtual void Update(const Time& deltaTime);

protected:
	BaseApp* mBaseApp;
	bool mEnabled;

private:
	BaseComponent(const BaseComponent& rhs);
	BaseComponent& operator=(const BaseComponent& rhs);
};