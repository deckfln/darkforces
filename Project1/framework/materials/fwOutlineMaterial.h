#pragma once
#include "../fwMaterial.h"
#include "../fwUniform.h"

class fwOutlineMaterial : public fwMaterial
{
	fwUniform *color = nullptr;

public:
	fwOutlineMaterial();
	fwOutlineMaterial(glm::vec4 *_color);
};
