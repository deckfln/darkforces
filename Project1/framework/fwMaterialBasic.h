#pragma once
#include "fwMaterial.h"
#include "fwUniform.h"
#include "fwTexture.h"

#define BASIC_MATERIAL 2

class fwMaterialBasic: public fwMaterial
{
public:
	fwMaterialBasic();
	fwMaterialBasic(glm::vec4 *_color);
	fwMaterialBasic(std::string vertex_shader, std::string forward_fragment, std::string defered_fragment);
	void addDiffuseMap(fwTexture *_diffuse);
	~fwMaterialBasic();
};

