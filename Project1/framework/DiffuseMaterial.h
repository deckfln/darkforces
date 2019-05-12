#pragma once
#include "Material.h"
#include "Texture.h"
#include "Uniform.h"

class DiffuseMaterial: public Material
{
	Texture *diffuse = nullptr;
	Texture *specular = nullptr;
	float shininess = 0;
	Uniform *uniform = nullptr;

public:
	DiffuseMaterial();
	DiffuseMaterial(Texture *diffuse, Texture *specular, float shininess);
	~DiffuseMaterial();
};

