#pragma once
#include "fwMaterial.h"
#include "Uniform.h"

class fwMaterialBasic: public fwMaterial
{
	Uniform *color= nullptr;

public:
	fwMaterialBasic();
	fwMaterialBasic(glm::vec4 *_color);
	~fwMaterialBasic();
};

