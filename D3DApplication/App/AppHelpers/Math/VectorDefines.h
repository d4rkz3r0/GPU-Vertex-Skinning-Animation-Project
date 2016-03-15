#pragma once
#include "../../../Shared/SharedUtils.h"

class Vector2Helper
{
public:
	static const XMFLOAT2 Zero;
	static const XMFLOAT2 One;

private:
	Vector2Helper();
	Vector2Helper(const Vector2Helper& rhs);
	Vector2Helper& operator=(const Vector2Helper& rhs) = delete;
};

class Vector3Helper
{
public:
	static const XMFLOAT3 Zero;
	static const XMFLOAT3 One;
	static const XMFLOAT3 Forward;
	static const XMFLOAT3 Backward;
	static const XMFLOAT3 Up;
	static const XMFLOAT3 Down;
	static const XMFLOAT3 Right;
	static const XMFLOAT3 Left;

private:
	Vector3Helper();
	Vector3Helper(const Vector3Helper& rhs);
	Vector3Helper& operator=(const Vector3Helper& rhs) = delete;
};

class Vector4Helper
{
public:
	static const XMFLOAT4 Zero;
	static const XMFLOAT4 One;

private:
	Vector4Helper();
	Vector4Helper(const Vector3Helper& rhs);
	Vector4Helper& operator=(const Vector3Helper& rhs) = delete;
};