#include "fwMaterialDiffuse.h"



fwMaterialDiffuse::fwMaterialDiffuse():
	diffuse(nullptr),
	specular(nullptr),
	shininess(0)
{
	addShader(VERTEX_SHADER, "shaders/vertex_diffuse.glsl");
	addShader(FRAGMENT_SHADER, "shaders/fragment_diffuse.glsl", FORWARD_RENDER);
	addShader(FRAGMENT_SHADER, "shaders/gbuffer/fragment.glsl", DEFERED_RENDER);

	m_defines = "#define MATERIAL_DIFFUSE\n";
	m_type |= DIFFUSE_MATERIAL;
}

fwMaterialDiffuse::fwMaterialDiffuse(fwTexture *_diffuse, float _shininess):
	diffuse(_diffuse),
	shininess(_shininess)
{
	m_type |= DIFFUSE_MATERIAL;

	addShader(VERTEX_SHADER, "shaders/vertex_diffuse.glsl");
	addShader(FRAGMENT_SHADER, "shaders/fragment_diffuse.glsl");
	addShader(FRAGMENT_SHADER, "shaders/gbuffer/fragment.glsl", DEFERED_RENDER);
	addTexture("material.diffuse", diffuse);

	fwUniform *uniform = new fwUniform("material.shininess", &shininess);
	addUniform(uniform);

	m_defines = "#define MATERIAL_DIFFUSE\n";
	m_type |= DIFFUSE_MATERIAL;
}

fwMaterialDiffuse & fwMaterialDiffuse::specularMap(fwTexture *_specular)
{
	specular = _specular;
	addTexture("material.specular", specular);
	m_defines += "#define SPECULAR_MAP\n";
	return *this;
}

fwMaterialDiffuse & fwMaterialDiffuse::normalMap(fwTexture *_normal)
{
	m_normalMap = _normal;
	addTexture("material.normalMap", m_normalMap);
	m_defines += "#define NORMALMAP\n";
	return *this;
}

fwTexture * fwMaterialDiffuse::normalMap(void)
{
	return m_normalMap;
}

fwMaterialDiffuse::~fwMaterialDiffuse()
{
	delete m_normalMap;
	delete specular;
}