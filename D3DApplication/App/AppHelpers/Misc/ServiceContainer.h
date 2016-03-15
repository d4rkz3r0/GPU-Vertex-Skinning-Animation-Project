#pragma once
#include "../../../Shared/SharedUtils.h"

class ServiceContainer
{
public:
	ServiceContainer();
	~ServiceContainer();

	void AddService(UINT typeID, void* service);
	void RemoveService(UINT typeID);
	void* GetService(UINT typeID) const;

private:
	//No Copy
	ServiceContainer(const ServiceContainer& rhs) = delete;
	ServiceContainer& operator=(const ServiceContainer& rhs) = delete;

	map<UINT, void*> mServices;
};
