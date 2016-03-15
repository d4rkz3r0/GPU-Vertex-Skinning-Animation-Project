#pragma once
#include "Material.h"
#include "../VertexStructs.h"
#include "../../../../Shared/SharedUtils.h"

class Mesh;

class SkinnedMaterial : public Material
{
	RTTI_DECLARATIONS(SkinnedMaterial, Material)
public:
	SkinnedMaterial();
	virtual void CreateVertexBuffer(ID3D11Device* device, const Mesh& mesh, ID3D11Buffer** vertexBuffer) const override;
	void CreateVertexBuffer(ID3D11Device* device, AnimationVertex* vertices, UINT vertexCount, ID3D11Buffer** vertexBuffer) const;
	virtual UINT VertexSize() const override;
};