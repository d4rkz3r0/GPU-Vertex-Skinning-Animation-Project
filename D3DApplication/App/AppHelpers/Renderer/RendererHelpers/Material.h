#pragma once
#include "../../../../Shared/SharedUtils.h"

//Forward Declaration
class Model;
class Mesh;

class Material : public RTTI
{
	RTTI_DECLARATIONS(Material, RTTI)

public:
	Material();
	virtual ~Material();

	virtual void CreateVertexBuffer(ID3D11Device* device, const Model& model, vector<ID3D11Buffer*>& vertexBuffers) const;
	virtual void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const = 0;
	virtual UINT VertexSize() const = 0;

protected:
	Material(const Material& rhs);
	Material& operator=(const Material& rhs);
};