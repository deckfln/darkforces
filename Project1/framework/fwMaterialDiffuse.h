#pragma once
#include "fwMaterial.h"
#include "fwTexture.h"
#include "fwUniform.h"

#define DIFFUSE_MATERIAL 1

class fwMaterialDiffuse: public fwMaterial
{
	fwTexture *diffuse = nullptr;
	fwTexture *specular = nullptr;
	fwTexture *m_normalMap = nullptr;
	float shininess = 0;

public:
	fwMaterialDiffuse();
	fwMaterialDiffuse(fwTexture *diffuse, float shininess);
	fwMaterialDiffuse(fwTextures* diffuse, float shininess);
	fwMaterialDiffuse &specularMap(fwTexture *_specular);
	fwMaterialDiffuse& specularMap(fwTextures* _specular);
	fwMaterialDiffuse &normalMap(fwTexture *_normal);
	fwTexture *normalMap(void);
	~fwMaterialDiffuse();
};

