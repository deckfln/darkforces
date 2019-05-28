#pragma once
#include "fwMaterial.h"
#include "fwTexture.h"
#include "fwUniform.h"

class fwDiffuseMaterial: public fwMaterial
{
	fwTexture *diffuse = nullptr;
	fwTexture *specular = nullptr;
	float shininess = 0;
	fwUniform *uniform = nullptr;

public:
	fwDiffuseMaterial();
	fwDiffuseMaterial(fwTexture *diffuse, fwTexture *specular, float shininess);
	~fwDiffuseMaterial();
};

