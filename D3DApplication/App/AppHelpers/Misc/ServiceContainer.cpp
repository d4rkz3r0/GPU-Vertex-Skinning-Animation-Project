#include "ServiceContainer.h"

ServiceContainer::ServiceContainer() : mServices() {}

ServiceContainer::~ServiceContainer() {}

void ServiceContainer::AddService(UINT typeID, void* service)
{
	mServices.insert(pair<UINT, void*>(typeID, service));
}

void ServiceContainer::RemoveService(UINT typeID)
{
	mServices.erase(typeID);
}

void* ServiceContainer::GetService(UINT typeID) const
{
	map<UINT, void*>::const_iterator iter = mServices.find(typeID);

	//If find() succeeded (aka is not at the end of it's map), then return the typeID's object, else return nullptr
	return (iter != mServices.end() ? iter->second : nullptr);
}