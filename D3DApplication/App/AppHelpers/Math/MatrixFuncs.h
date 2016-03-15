#pragma once
#include "../../../Shared/SharedUtils.h"

class MatrixFuncs
{
public:
	static const XMFLOAT4X4 Identity;
	static const XMFLOAT4X4 Zero;

	//CMatrix in parameter list can provide compiler optimizations through inlining,
	//or maybe not depends on the compiler.
	static void GetForward(CXMMATRIX matrix, XMFLOAT3 &vector);
	static void GetUp(CXMMATRIX matrix, XMFLOAT3 &vector);
	static void GetRight(CXMMATRIX matrix, XMFLOAT3 &vector);
	static void GetTranslation(CXMMATRIX matrix, XMFLOAT3 &vector);

	static void SetForward(XMMATRIX& matrix, XMFLOAT3 &forward);
	static void SetUp(XMMATRIX& matrix, XMFLOAT3 &up);
	static void SetRight(XMMATRIX& matrix, XMFLOAT3 &right);
	static void SetTranslation(XMMATRIX& matrix, XMFLOAT3 &translation);

private:
	MatrixFuncs() = default;
	MatrixFuncs(const MatrixFuncs& rhs);
	MatrixFuncs& operator=(const MatrixFuncs& rhs) = delete;
};