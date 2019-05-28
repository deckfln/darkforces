#pragma once
#include "fwMaterial.h"
#include "fwUniform.h"
#include "fwTexture.h"

class fwMaterialBasic: public fwMaterial
{
	fwUniform *color= nullptr;
	fwUniform *map = nullptr;

public:
	fwMaterialBasic();
	fwMaterialBasic(glm::vec4 *_color);
	void addDiffuseMap(fwTexture *_diffuse);
	~fwMaterialBasic();
};

