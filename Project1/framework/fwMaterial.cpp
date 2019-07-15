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
	id(_materialID++)
{
}

fwMaterial::fwMaterial(std::string _vertexShader, std::string _fragmentShader, std::string _geometryShader):
	id(_materialID++)
{
	addShader(VERTEX_SHADER, _vertexShader);
	addShader(GEOMETRY_SHADER, _geometryShader);
	addShader(FRAGMENT_SHADER, _fragmentShader);
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

/*
fwMaterial &fwMaterial::addShaders(std::string _vertexShader, std::string _fragmentShader, const std::string _defines)
{
	addShader(VERTEX_SHADER, _vertexShader);
	addShader(FRAGMENT_SHADER, _fragmentShader);

	m_defines = _defines;

	return *this;
}
*/
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
	for (auto uniform: uniforms) {
		uniform->set_uniform(program);
	}
}

void fwMaterial::bindTextures(void)
{
	for (auto texture: textures) {
		texture->bind();
	}
}

const std::string &fwMaterial::get_vertexShader(void)
{
	return get_shader(VERTEX_SHADER);
}

const std::string& fwMaterial::get_geometryShader(void)
{
	return get_shader(GEOMETRY_SHADER);
}

const std::string& fwMaterial::get_fragmentShader(void)
{
	return get_shader(FRAGMENT_SHADER);
}

fwMaterial &fwMaterial::addShader(int shader, std::string file, RenderType render)
{
	files[render][shader] = file; 
	return *this;
}

const std::string &fwMaterial::get_shader(int shader, RenderType render)
{
	std::string code = shaders[render][shader];

	if (code == "") {
		std::string file = files[render][shader];
		if (file == "") {
			return "";
		}
		code = shaders[render][shader] = load_shader_file(file, m_defines);
	}

	return code;
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