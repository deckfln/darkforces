#include "fwMaterial.h"
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "../glEngine/glTexture.h"

int _materialID = 0;

fwMaterial::fwMaterial():
	fragmentShaderCode(""),
	id(_materialID++)
{
}

fwMaterial::fwMaterial(std::string _vertexShader, std::string _fragmentShader, std::string _geometryShader):
	vertexShader(_vertexShader),
	fragmentShader(_fragmentShader),
	geometryShader(_geometryShader),
	id(_materialID++)
{
}

fwMaterial& fwMaterial::addTexture(std::string uniform, fwTexture *texture)
{
	glTexture *glTex = new glTexture(texture);
	textures.push_front(glTex);

	addUniform(new fwUniform(uniform, glTex));

	return *this;
}

fwMaterial& fwMaterial::addTexture(std::string uniform, glTexture *texture)
{
	textures.push_front(texture);

	addUniform(new fwUniform(uniform, texture));

	return *this;
}

fwMaterial &fwMaterial::addShaders(std::string _vertexShader, std::string _fragmentShader, const std::string _defines)
{
	vertexShader = _vertexShader;
	fragmentShader = _fragmentShader;
	m_defines = _defines;

	return *this;
}

fwMaterial &fwMaterial::addUniform(fwUniform *uniform)
{
	uniforms.push_front(uniform);
	return *this;
}

const int fwMaterial::getID(void)
{
	return id;
}

void fwMaterial::set_uniforms(glProgram *program)
{
	std::list <fwUniform *> ::iterator it;
	for (it = uniforms.begin(); it != uniforms.end(); ++it) {
		(*it)->set_uniform(program);
	}
}

void fwMaterial::bindTextures(void)
{
	std::list <glTexture *> ::iterator it;
	for (it = textures.begin(); it != textures.end(); ++it) {
		(*it)->bind();
	}
}

const std::string &fwMaterial::get_vertexShader(void)
{
	if (vertexShaderCode != "") {
		return vertexShaderCode;
	}
	vertexShaderCode = load_shader_file(vertexShader, m_defines);

	return vertexShaderCode;
}

fwMaterial &fwMaterial::addShader(const std::string name, std::string file)
{
	shaders[name] = file;
	return *this;
}

const std::string &fwMaterial::get_shader(const std::string name)
{
	if (shaderCode[name] != "") {
		return shaderCode[name];
	}
	shaderCode[name] = load_shader_file(shaders[name], m_defines);

	return shaderCode[name];
}

const std::string &fwMaterial::get_fragmentShader(void)
{
	if (fragmentShaderCode != "") {
		return fragmentShaderCode;
	}
	fragmentShaderCode = load_shader_file(fragmentShader, m_defines);

	return fragmentShaderCode;
}

const std::string &fwMaterial::get_geometryShader(void)
{
	if (geometryShader == "") {
		return geometryShaderCode;
	}

	if (geometryShaderCode != "") {
		return geometryShaderCode;
	}
	geometryShaderCode = load_shader_file(geometryShader, m_defines);

	return geometryShaderCode;
}

std::string fwMaterial::hashCode(void)
{
	return vertexShader+fragmentShader+m_defines;
}

fwMaterial::~fwMaterial()
{
	for (auto texture: textures) {
		delete texture;
	}

	for (auto uniform : uniforms) {
		delete uniform;
	}
}