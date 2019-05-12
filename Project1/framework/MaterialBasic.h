#pragma once
#include "Material.h"
#include "Uniform.h"

class MaterialBasic: public Material
{
	Uniform *color= nullptr;

public:
	MaterialBasic();
	MaterialBasic(glm::vec4 *_color);
	~MaterialBasic();
};

