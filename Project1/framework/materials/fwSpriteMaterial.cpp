#include "fwSpriteMaterial.h"

fwSpriteMaterial::fwSpriteMaterial():
	fwMaterial("shaders/sprite/vertex.glsl", "shaders/sprite/fragment.glsl", "shaders/sprite/geometry.glsl")
{

}

fwSpriteMaterial::~fwSpriteMaterial()
{

}