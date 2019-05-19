#include "OutlineMaterial.h"


OutlineMaterial::OutlineMaterial() :
	Material("shaders/vertex_outline.glsl", "shaders/fragment_outline.glsl", "")
{
}

OutlineMaterial::OutlineMaterial(glm::vec4 *_color):
	Material("shaders/vertex_outline.glsl", "shaders/fragment_outline.glsl", "")
{
	color = new Uniform("color", _color);
	addUniform(color);
}

OutlineMaterial::~OutlineMaterial()
{
}
