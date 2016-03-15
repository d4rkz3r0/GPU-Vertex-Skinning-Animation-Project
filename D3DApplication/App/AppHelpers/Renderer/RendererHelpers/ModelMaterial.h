#pragma once
#include "../../../../Shared/SharedUtils.h"

//Forward Declaration
struct aiMaterial;

enum TextureType
{
	TextureTypeDiffuse = 0,
	TextureTypeSpecularMap,
	TextureTypeAmbient,
	TextureTypeEmissive,
	TextureTypeHeightmap,
	TextureTypeNormalMap,
	TextureTypeSpecularPowerMap,
	TextureTypeDisplacementMap,
	TextureTypeLightMap,
	TextureTypeEnd
};

class ModelMaterial
{
	friend class Model;

public:
	ModelMaterial(Model& model);
	~ModelMaterial();

	Model& GetModel();
	const string& Name() const;
	const map<TextureType, vector<wstring>*>& Textures() const;

private:
	static void InitializeTextureTypeMappings();
	static map<TextureType, UINT> sTextureTypeMappings;

	ModelMaterial(Model& model, aiMaterial* material);
	ModelMaterial(const ModelMaterial& rhs);
	ModelMaterial& operator=(const ModelMaterial& rhs);

	Model& mModel;
	string mName;
	map<TextureType, vector<wstring>*> mTextures;
};