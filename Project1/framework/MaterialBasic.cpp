#include "MaterialBasic.h"


MaterialBasic::MaterialBasic() :
	Material("shaders/vertex_basic.glsl", "shaders/fragment_basic.glsl")
{
}

MaterialBasic::MaterialBasic(glm::vec4 *_color):
	Material("shaders/vertex_basic.glsl", "shaders/fragment_basic.glsl")
{
	color = new Uniform("color", _color);
	addUniform(color);
}


MaterialBasic::~MaterialBasic()
{
}
