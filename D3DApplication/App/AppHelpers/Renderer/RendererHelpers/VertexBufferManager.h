#pragma once
#include <d3d11.h>
#include "VertexBuffer.h"
#include "../VertexStructs.h"

class VertexBufferManager
{
public:

	VertexBufferManager()
	{
		
	}

	VertexBufferManager(ID3D11Device* theDevice);
	~VertexBufferManager();

	inline VertexBuffer<NormalVertex>& GetNormalVertexBuffer() { return mNormalVertexBuffer; }

	template <typename VertexType>
	VertexBuffer<VertexType> &GetVertexBuffer();

	void SetDevice(ID3D11Device* theDevice);
	void SetDeviceContext(ID3D11DeviceContext* theDeviceContext);

	void Destroy();

public:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;

	//Vertex Buffers By VertexType
	VertexBuffer<NormalVertex> mNormalVertexBuffer;
};

template <>
inline VertexBuffer<NormalVertex>& VertexBufferManager::GetVertexBuffer()
{
	return GetNormalVertexBuffer();
}