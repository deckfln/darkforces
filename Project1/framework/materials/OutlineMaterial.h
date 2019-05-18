#pragma once
#include "../Material.h"
#include "../Uniform.h"

class OutlineMaterial : public Material
{
	Uniform *color = nullptr;

public:
	OutlineMaterial();
	OutlineMaterial(glm::vec4 *_color);
	~OutlineMaterial();
};