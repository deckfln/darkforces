#include "fwDiffuseMaterial.h"



fwDiffuseMaterial::fwDiffuseMaterial():
	diffuse(nullptr),
	specular(nullptr),
	shininess(0)
{
	addShader(VERTEX_SHADER, "shaders/vertex_diffuse.glsl");
	addShader(FRAGMENT_SHADER, "shaders/fragment_diffuse.glsl");

	m_type |= DIFFUSE_MATERIAL;
}

fwDiffuseMaterial::fwDiffuseMaterial(fwTexture *_diffuse, float _shininess):
	diffuse(_diffuse),
	shininess(_shininess)
{
	m_type |= DIFFUSE_MATERIAL;

	addShader(VERTEX_SHADER, "shaders/vertex_diffuse.glsl");
	addShader(FRAGMENT_SHADER, "shaders/fragment_diffuse.glsl");
	addTexture("material.diffuse", diffuse);

	uniform = new fwUniform("material.shininess", &shininess);
	addUniform(uniform);
}

fwDiffuseMaterial &fwDiffuseMaterial::specularMap(fwTexture *_specular)
{
	specular = _specular;
	addTexture("material.specular", specular);
	m_defines += "#define SPECULAR_MAP\n";
	return *this;
}

fwDiffuseMaterial &fwDiffuseMaterial::normalMap(fwTexture *_normal)
{
	m_normalMap = _normal;
	addTexture("material.normalMap", m_normalMap);
	m_defines += "#define NORMALMAP\n";
	return *this;
}

fwTexture *fwDiffuseMaterial::normalMap(void)
{
	return m_normalMap;
}

fwDiffuseMaterial::~fwDiffuseMaterial()
{
	delete m_normalMap;
	delete specular;
}