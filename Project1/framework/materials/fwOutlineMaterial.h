#pragma once
#include "../fwMaterial.h"
#include "../Uniform.h"

class fwOutlineMaterial : public fwMaterial
{
	Uniform *color = nullptr;

public:
	fwOutlineMaterial();
	fwOutlineMaterial(glm::vec4 *_color);
	~fwOutlineMaterial();
};
