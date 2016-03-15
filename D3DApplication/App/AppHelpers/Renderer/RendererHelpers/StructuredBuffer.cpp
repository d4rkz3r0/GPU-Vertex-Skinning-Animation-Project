#include "StructuredBuffer.h"

StructuredBuffer::StructuredBuffer(ID3D11Device* pDevice, UINT bindFlags, const void* data, size_t count, UINT stride, CPUAccess cpuAccess, bool bUAV) : mDevice(pDevice), mStride(stride), mBindFlags(bindFlags), mCount(static_cast<UINT>(count)), mIsDirty(false), mCPUAccess(cpuAccess), mNeedUAV(bUAV)
{
	mBuffer = nullptr;
	mSRV = nullptr;
	mUAV = nullptr;

	mNeedDynamic = static_cast<int>(mCPUAccess) != 0;
	mNeedUAV = mNeedUAV && !mNeedDynamic;
	size_t numBytes = mCount * mStride;

	if (data)
	{
		mData.assign((uint8_t*)data, (uint8_t*)data + numBytes);
	}
	else
	{
		mData.reserve(numBytes);
	}

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = (UINT)numBytes;

	if (((int)mCPUAccess & (int)CPUAccess::Read) != 0)
	{
		bufferDesc.Usage = D3D11_USAGE_STAGING;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}
	else if (((int)mCPUAccess & (int)CPUAccess::Write) != 0)
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}
	else
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if (mNeedUAV)
		{
			bufferDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		}
	}

	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = mStride;

	D3D11_SUBRESOURCE_DATA subResourceData;
	subResourceData.pSysMem = (void*)mData.data();
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	mDevice->CreateBuffer(&bufferDesc, &subResourceData, &mBuffer);

	if ((bufferDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) != 0)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = mCount;
		mDevice->CreateShaderResourceView(mBuffer, &srvDesc, &mSRV);
	}

	if ((bufferDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) != 0)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = mCount;
		uavDesc.Buffer.Flags = 0;

		mDevice->CreateUnorderedAccessView(mBuffer, &uavDesc, &mUAV);
	}

	mDevice->GetImmediateContext(&mDeviceContext);
}

StructuredBuffer::~StructuredBuffer()
{
	mDevice->Release();
	mDeviceContext->Release();
	mBuffer->Release();
	mSRV->Release();
	mUAV->Release();
}

bool StructuredBuffer::Bind(UINT id, Shader::ShaderType shaderType, UINT parameterType)
{
	if (mIsDirty)
	{
		Commit();
		mIsDirty = false;
	}

	if (parameterType == 3 && mSRV)
	{
		ID3D11ShaderResourceView* srv[] = { mSRV };

		switch (shaderType)
		{
		case Shader::VertexShader:
			mDeviceContext->VSSetShaderResources(id, 1, srv);
			break;
		case Shader::PixelShader:
			mDeviceContext->PSSetShaderResources(id, 1, srv);
			break;
		}
	}

	return true;
}

void StructuredBuffer::UnBind(UINT id, Shader::ShaderType shaderType, UINT parameterType)
{
	ID3D11UnorderedAccessView* uav[] = { nullptr };
	ID3D11ShaderResourceView* srv[] = { nullptr };

	if (parameterType == 3)
	{
		switch (shaderType)
		{
		case Shader::VertexShader:
			mDeviceContext->VSSetShaderResources(id, 1, srv);
			break;
		case Shader::PixelShader:
			mDeviceContext->PSSetShaderResources(id, 1, srv);
			break;
		}
	}
}

StructuredBuffer::BufferType StructuredBuffer::GetType() const
{
	return StructuredBufferType;
}

unsigned StructuredBuffer::GetElementCount() const
{
	return mCount;
}

void StructuredBuffer::Copy(StructuredBuffer* other)
{
	StructuredBuffer* srcBuffer = (StructuredBuffer*)other;

	if (srcBuffer->mIsDirty)
	{
		srcBuffer->Commit();
	}

	if (srcBuffer && srcBuffer != this && mCount * mStride == srcBuffer->mCount * srcBuffer->mStride)
	{
		mDeviceContext->CopyResource(mBuffer, srcBuffer->mBuffer);
	}

	if (((int)mCPUAccess & (int)CPUAccess::Read) != 0)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		mDeviceContext->Map(mBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
		memcpy_s(mData.data(), mData.size(), mappedResource.pData, mData.size());
		mDeviceContext->Unmap(mBuffer, 0);
	}
}

void StructuredBuffer::Clear()
{

}

ID3D11UnorderedAccessView* StructuredBuffer::GetUnorderedAccessView() const
{
	return mUAV;
}

void StructuredBuffer::Copy(BufferType* other)
{
	if (mNeedUAV)
	{
		FLOAT clearColor[4] = { 0, 0, 0, 0 };
		mDeviceContext->ClearUnorderedAccessViewFloat(mUAV, clearColor);
	}
}

void StructuredBuffer::SetData(void* data, size_t elementSize, size_t offset, size_t numElements)
{
	unsigned char* first = (unsigned char*)data + (offset * elementSize);
	unsigned char* last = first + (numElements * elementSize);
	mData.assign(first, last);
}

void StructuredBuffer::Commit()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	mDeviceContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	size_t sizeInBytes = mData.size();
	memcpy_s(mappedResource.pData, sizeInBytes, mData.data(), sizeInBytes);
	mDeviceContext->Unmap(mBuffer, 0);
}