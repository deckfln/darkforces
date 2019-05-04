#pragma once
#include "Material.h"
#include "Texture.h"
#include "Uniform.h"

class DiffuseMaterial: public Material
{
	Texture *diffuse;
	Texture *specular;
	float shininess;
	Uniform uniform;

public:
	DiffuseMaterial();
	DiffuseMaterial(Texture *diffuse, Texture *specular, float shininess);
	~DiffuseMaterial();
};

