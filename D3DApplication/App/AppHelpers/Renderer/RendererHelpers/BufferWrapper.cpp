#include "BufferWrapper.h"
#include "../../../../Shared/SharedUtils.h"

BufferWrapper::BufferWrapper() : mBuffer(nullptr), mElementCount(0)
{ }

ID3D11Buffer* BufferWrapper::GetBuffer()
{
	return mBuffer;
}

void BufferWrapper::SetBuffer(ID3D11Buffer* buffer)
{
	mBuffer = buffer;
}

UINT BufferWrapper::ElementCount() const
{
	return mElementCount;
}

void BufferWrapper::SetElementCount(UINT elementCount)
{
	mElementCount = elementCount;
}

void BufferWrapper::ReleaseBuffer()
{
	ReleaseObject(mBuffer);
	mElementCount = 0;
}