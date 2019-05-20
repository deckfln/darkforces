#pragma once
#include "fwMaterial.h"
#include "Texture.h"
#include "Uniform.h"

class fwDiffuseMaterial: public fwMaterial
{
	Texture *diffuse = nullptr;
	Texture *specular = nullptr;
	float shininess = 0;
	Uniform *uniform = nullptr;

public:
	fwDiffuseMaterial();
	fwDiffuseMaterial(Texture *diffuse, Texture *specular, float shininess);
	~fwDiffuseMaterial();
};

