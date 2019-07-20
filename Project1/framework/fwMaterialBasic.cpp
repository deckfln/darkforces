#include "fwMaterialBasic.h"


fwMaterialBasic::fwMaterialBasic() :
	fwMaterial("shaders/vertex_basic.glsl", "shaders/fragment_basic.glsl", "")
{
	addShader(FRAGMENT_SHADER, "shaders/gbuffer/basic_fs.glsl", DEFERED_RENDER);

	m_defines = "#define BASIC_MATERIAL\n";
	m_type |= BASIC_MATERIAL;
}

fwMaterialBasic::fwMaterialBasic(glm::vec4 *_color):
	fwMaterial("shaders/vertex_basic.glsl", "shaders/fragment_basic.glsl", "")
{
	addShader(FRAGMENT_SHADER, "shaders/gbuffer/basic_fs.glsl", DEFERED_RENDER);

	m_defines = "#define BASIC_MATERIAL\n";
	m_type |= BASIC_MATERIAL;

	fwUniform *uniform = new fwUniform("material.color", _color);
	addUniform(uniform);
}

void fwMaterialBasic::addDiffuseMap(fwTexture *_diffuse)
{
	m_defines += "#define DIFFUSE_MAP\n";

	addTexture("material.diffuse", _diffuse);
}

fwMaterialBasic::~fwMaterialBasic()
{
}
