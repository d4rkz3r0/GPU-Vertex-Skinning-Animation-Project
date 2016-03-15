#include "VertexBufferManager.h"


VertexBufferManager::~VertexBufferManager()
{
	Destroy();
}

VertexBufferManager::VertexBufferManager(ID3D11Device* theDevice) : mDevice(theDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);
}

void VertexBufferManager::SetDevice(ID3D11Device* theDevice)
{
		
}

void VertexBufferManager::SetDeviceContext(ID3D11DeviceContext* theDeviceContext)
{
	
}

void VertexBufferManager::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
}