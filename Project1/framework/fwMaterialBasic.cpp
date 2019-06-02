#include "fwMaterialBasic.h"


fwMaterialBasic::fwMaterialBasic() :
	fwMaterial("shaders/vertex_basic.glsl", "shaders/fragment_basic.glsl", "")
{
}

fwMaterialBasic::fwMaterialBasic(glm::vec4 *_color):
	fwMaterial("shaders/vertex_basic.glsl", "shaders/fragment_basic.glsl", "")
{
	color = new fwUniform("color", _color);
	addUniform(color);
}

void fwMaterialBasic::addDiffuseMap(fwTexture *_diffuse)
{
	addTexture("map", _diffuse);
	m_defines += "#define DIFFUSE_MAP\n";
}

fwMaterialBasic::~fwMaterialBasic()
{
	if (color != nullptr)
		delete color;

	if (map != nullptr)
		delete map;
}
