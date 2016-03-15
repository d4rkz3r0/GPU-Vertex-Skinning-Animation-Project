#pragma once
#include <d3d11.h>
#include "Shader.h"
#include <vector>

class StructuredBuffer
{
public:
	enum class CPUAccess
	{
		None = 0,
		Read = (1 << 0),
		Write = (1 << 1),
		ReadWrite = Read | Write
	};

	enum BufferType
	{
		VertexBufferType,
		IndexBufferType,
		StructuredBufferType,
		ConstantBufferType
	};

	StructuredBuffer(ID3D11Device* pDevice, UINT bindFlags, const void* data, size_t count, UINT stride, CPUAccess cpuAccess = CPUAccess::None, bool bUAV = false);
	virtual ~StructuredBuffer();

	virtual bool Bind(UINT id, Shader::ShaderType shaderType, UINT parameterType);
	virtual void UnBind(UINT id, Shader::ShaderType shaderType, UINT parameterType);

	virtual BufferType GetType() const;
	virtual unsigned int GetElementCount() const;
	virtual void Copy(StructuredBuffer* other);

	template<typename T>
	void Set(const std::vector<T>& values);

	virtual void Clear();

	ID3D11UnorderedAccessView* GetUnorderedAccessView() const;

protected:
	virtual void Copy(BufferType* other);
	virtual void SetData(void* data, size_t elementSize, size_t offset, size_t numElements);
	void Commit();

private:
	ID3D11Device* mDevice;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mBuffer;
	ID3D11ShaderResourceView* mSRV;
	ID3D11UnorderedAccessView* mUAV;

	typedef std::vector<uint8_t> BufferTypeData;
	BufferTypeData mData;
	CPUAccess mCPUAccess;
	UINT mStride;
	UINT mBindFlags;
	UINT mCount;
	UINT mSlot;
	bool mNeedUAV;
	bool mNeedDynamic;
	bool mIsDirty;
};

template <typename T>
void StructuredBuffer::Set(const std::vector<T>& values)
{
	SetData(static_cast<void*>(values.data()), sizeof(T), 0, values.size());
}