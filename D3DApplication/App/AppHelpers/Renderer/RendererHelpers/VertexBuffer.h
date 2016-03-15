#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Renderer.h"
#include "../VertexStructs.h"
#include "../../../../Shared/SharedUtils.h"
using namespace std;

template <typename VertexType>
class VertexBuffer
{
	friend class VertexBufferManager;
	friend class Renderer;

public:
	VertexBuffer();
	VertexBuffer(ID3D11Device* pDevice);
	~VertexBuffer();
	VertexBuffer(const VertexBuffer&) {};
	VertexBuffer &operator=(const VertexBuffer&) { return *this; }

	UINT AddVerts(const VertexType* vertices, UINT numVertsToAdd);
	inline ID3D11Buffer* const GetVertexBuffer() { return mVertexBuffer; }
	void Destroy();

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mVertexBuffer;
};

template <typename VertexType>
UINT VertexBuffer<VertexType>::AddVerts(const VertexType* vertices, UINT numVertsToAdd)
{
	size_t startingVertIndex;

	//First Vertices To Add
	if (!mVertexBuffer)
	{
		startingVertIndex = 0;

		D3D11_BUFFER_DESC vbd;
		vbd.Usage = D3D11_USAGE_DEFAULT;
		vbd.ByteWidth = sizeof(VertexType) * numVertsToAdd;
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		vbd.MiscFlags = 0;
		vbd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA vinitData;
		vinitData.pSysMem = vertices;
		mDevice->CreateBuffer(&vbd, &vinitData, &mVertexBuffer);
	}
	else
	{
		D3D11_BUFFER_DESC ibd;

		mVertexBuffer->GetDesc(&ibd);
		D3D11_SUBRESOURCE_DATA iinitData;
		UINT oldBufferSize = ibd.ByteWidth / sizeof(VertexType);
		UINT newBufferSize = oldBufferSize + numVertsToAdd;

		iinitData.pSysMem = new VertexType[newBufferSize];
		memcpy((char *)(iinitData.pSysMem) + ibd.ByteWidth, vertices, sizeof(VertexType) * numVertsToAdd);
		ibd.ByteWidth += sizeof(VertexType) * numVertsToAdd;

		ID3D11Buffer* newVertexBuffer;
		Renderer::mD3DDevice->CreateBuffer(&ibd, &iinitData, &newVertexBuffer);
		Renderer::mD3DContext->CopySubresourceRegion(newVertexBuffer, 0, 0, 0, 0, mVertexBuffer, 0, 0);
		ReleaseObject(mVertexBuffer);

		mVertexBuffer = newVertexBuffer;

		delete[] iinitData.pSysMem;
		startingVertIndex = oldBufferSize;
	}
	return static_cast<UINT>(startingVertIndex);
}

template <typename VertexType>
void VertexBuffer<VertexType>::Destroy()
{
	ReleaseObject(mDevice);
	ReleaseObject(mDeviceContext);
	ReleaseObject(mVertexBuffer);
}

template <typename VertexType>
VertexBuffer<VertexType>::VertexBuffer()
{
	mVertexBuffer = nullptr;
}

template <typename VertexType>
VertexBuffer<VertexType>::VertexBuffer(ID3D11Device* pDevice) : mDevice(pDevice)
{
	mDevice->GetImmediateContext(&mDeviceContext);
	mVertexBuffer = nullptr;
}

template <typename VertexType>
VertexBuffer<VertexType>::~VertexBuffer()
{
	Destroy();
}