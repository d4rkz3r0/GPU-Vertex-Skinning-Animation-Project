#include "Material.h"
#include "Model.h"


RTTI_DEFINITIONS(Material)

Material::Material()
{ }


Material::~Material()
{ }

void Material::CreateVertexBuffer(ID3D11Device* device, const Model& model, vector<ID3D11Buffer*>& vertexBuffers) const
{
	vertexBuffers.reserve(model.Meshes().size());
	for (Mesh* mesh : model.Meshes())
	{
		ID3D11Buffer* vertexBuffer;
		CreateVertexBuffer(device, *mesh, &vertexBuffer);
		vertexBuffers.push_back(vertexBuffer);
	}
}