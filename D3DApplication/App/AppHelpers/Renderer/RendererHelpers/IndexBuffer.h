#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Renderer.h"
#include "../../../../Shared/SharedUtils.h"

class IndexBuffer
{
public:
	IndexBuffer(ID3D11Device* pDevice);
	~IndexBuffer();
	IndexBuffer(const IndexBuffer&) {}
	IndexBuffer &operator=(const IndexBuffer&) { return *this; }

	UINT AddIndices(const UINT* indices, UINT numIndicesToAdd);
	inline ID3D11Buffer* const GetIndices() { return mIndexBuffer; }
	void Destroy();

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mIndexBuffer;
};

inline IndexBuffer::IndexBuffer(ID3D11Device* pDevice) : mDevice(pDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);
	mIndexBuffer = nullptr;
}

inline IndexBuffer::~IndexBuffer()
{
	Destroy();
}

inline UINT IndexBuffer::AddIndices(const UINT* indices, UINT numIndicesToAdd)
{
	size_t startingIndex;

	//First Indices To Add
	if (!mIndexBuffer)
	{
		startingIndex = 0;

		D3D11_BUFFER_DESC ibd;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.ByteWidth = sizeof(UINT) * numIndicesToAdd;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		ibd.MiscFlags = 0;
		ibd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA iinitData;
		iinitData.pSysMem = indices;
		mDevice->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);
	}
	else
	{
		D3D11_BUFFER_DESC ibd;

		mIndexBuffer->GetDesc(&ibd);
		D3D11_SUBRESOURCE_DATA iinitData;
		UINT oldBufferSize = ibd.ByteWidth / sizeof(UINT);
		UINT newBufferSize = oldBufferSize + numIndicesToAdd;

		iinitData.pSysMem = new UINT[newBufferSize];
		memcpy((char *)(iinitData.pSysMem) + ibd.ByteWidth, indices, sizeof(UINT) * numIndicesToAdd);
		ibd.ByteWidth += sizeof(UINT) * numIndicesToAdd;

		ID3D11Buffer* newIndexBuffer;
		mDevice->CreateBuffer(&ibd, &iinitData, &newIndexBuffer);
		mDeviceContext->CopySubresourceRegion(newIndexBuffer, 0, 0, 0, 0, mIndexBuffer, 0, 0);
		ReleaseObject(mIndexBuffer);

		mIndexBuffer = newIndexBuffer;

		delete[] iinitData.pSysMem;
		startingIndex = oldBufferSize;
	}
	return static_cast<UINT>(startingIndex);
}

inline void IndexBuffer::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
	ReleaseObject(mIndexBuffer);
}