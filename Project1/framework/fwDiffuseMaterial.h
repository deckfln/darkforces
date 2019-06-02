#pragma once
#include "fwMaterial.h"
#include "fwTexture.h"
#include "fwUniform.h"

#define DIFFUSE_MATERIAL 1

class fwDiffuseMaterial: public fwMaterial
{
	fwTexture *diffuse = nullptr;
	fwTexture *specular = nullptr;
	fwTexture *m_normalMap = nullptr;
	float shininess = 0;
	fwUniform *uniform = nullptr;

public:
	fwDiffuseMaterial();
	fwDiffuseMaterial(fwTexture *diffuse, float shininess);
	fwDiffuseMaterial &specularMap(fwTexture *_specular);
	fwDiffuseMaterial &normalMap(fwTexture *_normal);
	fwTexture *normalMap(void);
	~fwDiffuseMaterial();
};

