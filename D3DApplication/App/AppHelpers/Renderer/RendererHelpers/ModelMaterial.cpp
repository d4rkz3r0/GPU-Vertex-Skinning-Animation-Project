#include "ModelMaterial.h"
#include "../../../../Shared/SharedUtils.h"
#include <assimp/scene.h>

map<TextureType, UINT> ModelMaterial::sTextureTypeMappings;

ModelMaterial::ModelMaterial(Model& model) : mModel(model), mTextures()
{
	InitializeTextureTypeMappings();
}

ModelMaterial::ModelMaterial(Model& model, aiMaterial* material) : mModel(model), mTextures()
{
	InitializeTextureTypeMappings();

	aiString name;
	material->Get(AI_MATKEY_NAME, name);
	mName = name.C_Str();

	for (TextureType textureType = (TextureType)0; textureType < TextureTypeEnd; textureType = (TextureType)(textureType + 1))
	{
		aiTextureType mappedTextureType = (aiTextureType)sTextureTypeMappings[textureType];

		UINT textureCount = material->GetTextureCount(mappedTextureType);
		if (textureCount > 0)
		{
			vector<wstring>* textures = new vector<wstring>();
			mTextures.insert(pair<TextureType, vector<wstring>*>(textureType, textures));

			textures->reserve(textureCount);
			for (UINT textureIndex = 0; textureIndex < textureCount; textureIndex++)
			{
				aiString path;
				if (material->GetTexture(mappedTextureType, textureIndex, &path) == AI_SUCCESS)
				{
					wstring wPath;
					Utility::ToWideString(path.C_Str(), wPath);

					textures->push_back(wPath);
				}
			}
		}
	}
}

ModelMaterial::~ModelMaterial()
{
	for (pair<TextureType, vector<wstring>*> textures : mTextures)
	{
		DeleteObject(textures.second);
	}
}

Model& ModelMaterial::GetModel()
{
	return mModel;
}

const string& ModelMaterial::Name() const
{
	return mName;
}

const map<TextureType, vector<wstring>*>& ModelMaterial::Textures() const
{
	return mTextures;
}

void ModelMaterial::InitializeTextureTypeMappings()
{
	if (sTextureTypeMappings.size() != TextureTypeEnd)
	{
		sTextureTypeMappings[TextureTypeDiffuse] = aiTextureType_DIFFUSE;
		sTextureTypeMappings[TextureTypeSpecularMap] = aiTextureType_SPECULAR;
		sTextureTypeMappings[TextureTypeAmbient] = aiTextureType_AMBIENT;
		sTextureTypeMappings[TextureTypeHeightmap] = aiTextureType_HEIGHT;
		sTextureTypeMappings[TextureTypeNormalMap] = aiTextureType_NORMALS;
		sTextureTypeMappings[TextureTypeSpecularPowerMap] = aiTextureType_SHININESS;
		sTextureTypeMappings[TextureTypeDisplacementMap] = aiTextureType_DISPLACEMENT;
		sTextureTypeMappings[TextureTypeLightMap] = aiTextureType_LIGHTMAP;
	}
}