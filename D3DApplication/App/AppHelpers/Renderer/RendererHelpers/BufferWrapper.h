#pragma once
#include <d3d11.h>

class BufferWrapper
{
public:
	BufferWrapper();

	ID3D11Buffer* GetBuffer();
	void SetBuffer(ID3D11Buffer* buffer);

	UINT ElementCount() const;
	void SetElementCount(UINT elementCount);

	void ReleaseBuffer();

private:
	BufferWrapper(const BufferWrapper& rhs);
	BufferWrapper& operator=(const BufferWrapper& rhs);

	ID3D11Buffer* mBuffer;
	UINT mElementCount;
};