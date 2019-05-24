#pragma once
#include "fwMaterial.h"
#include "Uniform.h"
#include "Texture.h"

class fwMaterialBasic: public fwMaterial
{
	Uniform *color= nullptr;
	Uniform *map = nullptr;

public:
	fwMaterialBasic();
	fwMaterialBasic(glm::vec4 *_color);
	void addDiffuseMap(Texture *_diffuse);
	~fwMaterialBasic();
};

