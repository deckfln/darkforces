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

fwMaterialBasic::fwMaterialBasic(bool withColors) :
	fwMaterial("shaders/vertex_basic.glsl", "shaders/fragment_basic.glsl", "")
{
	addShader(FRAGMENT_SHADER, "shaders/gbuffer/basic_fs.glsl", DEFERED_RENDER);

	if (withColors) {
		m_defines = "#define BASIC_MATERIAL\n#define COLORS\n";
	}
	else {
		m_defines = "#define BASIC_MATERIAL\n";
	}
	m_type |= BASIC_MATERIAL;
}

fwMaterialBasic::fwMaterialBasic(std::string vertex_shader, std::string forward_fragment, std::string defered_fragment) :
	fwMaterial(vertex_shader, forward_fragment, "")
{
	addShader(FRAGMENT_SHADER, defered_fragment, DEFERED_RENDER);

	m_defines = "#define BASIC_MATERIAL\n";
	m_type |= BASIC_MATERIAL;
}

/**
 * Create a Basic Material 
 */
fwMaterialBasic::fwMaterialBasic(std::map<ShaderType, std::string>& shaders):
	fwMaterial(shaders)
{
	m_defines = "#define BASIC_MATERIAL\n";
	m_type |= BASIC_MATERIAL;
}

void fwMaterialBasic::addDiffuseMap(fwTexture *_diffuse)
{
	m_defines += "#define DIFFUSE_MAP\n";

	addTexture("material.diffuse", _diffuse);
}

fwMaterialBasic::~fwMaterialBasic()
{
}
