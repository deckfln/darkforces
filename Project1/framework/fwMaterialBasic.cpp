#include "fwMaterialBasic.h"


fwMaterialBasic::fwMaterialBasic() :
	fwMaterial("shaders/vertex_basic.glsl", "shaders/fragment_basic.glsl", "")
{
}

fwMaterialBasic::fwMaterialBasic(glm::vec4 *_color):
	fwMaterial("shaders/vertex_basic.glsl", "shaders/fragment_basic.glsl", "")
{
	color = new Uniform("color", _color);
	addUniform(color);
}


fwMaterialBasic::~fwMaterialBasic()
{
}
