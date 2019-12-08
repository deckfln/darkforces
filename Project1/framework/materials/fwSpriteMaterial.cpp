#include "fwSpriteMaterial.h"

fwSpriteMaterial::fwSpriteMaterial():
	fwMaterial("shaders/sprite/vertex.glsl", "shaders/sprite/fragment.glsl")
{
	addShader(FRAGMENT_SHADER, "shaders/sprite/fragment_defered.glsl", DEFERED_RENDER);
}

fwSpriteMaterial::~fwSpriteMaterial()
{

}