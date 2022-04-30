#include "fwOutlineMaterial.h"


fwOutlineMaterial::fwOutlineMaterial() :
	fwMaterial("shaders/vertex_outline.glsl", "shaders/fragment_outline.glsl", "")
{
}

fwOutlineMaterial::fwOutlineMaterial(glm::vec4 *_color):
	fwMaterial("shaders/vertex_outline.glsl", "shaders/fragment_outline.glsl", "")
{
	color = new fwUniform("color", _color);
	addUniform(color);
}
