#include "Material.h"
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "glEngine/glTexture.h"

int _materialID = 0;

Material::Material():
	fragmentShaderCode(""),
	id(_materialID++)
{
}

Material::Material(std::string _vertexShader, std::string _fragmentShader):
	vertexShader(_vertexShader),
	fragmentShader(_fragmentShader),
	vertexShaderCode(""),
	fragmentShaderCode(""),
	id(_materialID++)
{
}

Material& Material::addTexture(std::string uniform, Texture *texture)
{
	glTexture *glTex = new glTexture(texture);
	textures.push_front(glTex);

	addUniform(new Uniform(uniform, glTex));

	return *this;
}

Material& Material::addTexture(std::string uniform, glTexture *texture)
{
	textures.push_front(texture);

	addUniform(new Uniform(uniform, texture));

	return *this;
}

Material &Material::addShaders(std::string _vertexShader, std::string _fragmentShader, const std::string _defines)
{
	vertexShader = _vertexShader;
	fragmentShader = _fragmentShader;
	defines = _defines;

	return *this;
}

Material &Material::addUniform(Uniform *uniform)
{
	uniforms.push_front(uniform);
	return *this;
}

const int Material::getID(void)
{
	return id;
}

void Material::set_uniforms(glProgram *program)
{
	std::list <Uniform *> ::iterator it;
	for (it = uniforms.begin(); it != uniforms.end(); ++it) {
		(*it)->set_uniform(program);
	}
}

void Material::bindTextures(void)
{
	std::list <glTexture *> ::iterator it;
	for (it = textures.begin(); it != textures.end(); ++it) {
		(*it)->bind();
	}
}

const std::string &Material::get_vertexShader(void)
{
	if (vertexShaderCode != "") {
		return vertexShaderCode;
	}
	vertexShaderCode = load_shader_file(vertexShader, defines);

	return vertexShaderCode;
}

const std::string &Material::get_fragmentShader(void)
{
	if (fragmentShaderCode != "") {
		return fragmentShaderCode;
	}
	fragmentShaderCode = load_shader_file(fragmentShader, defines);

	return fragmentShaderCode;
}

std::string Material::hashCode(void)
{
	return vertexShader+fragmentShader;
}

Material::~Material()
{
	for (auto texture: textures) {
		delete texture;
	}

	for (auto uniform : uniforms) {
		delete uniform;
	}
}