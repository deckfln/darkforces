#include "Material.h"
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>

#include "glEngine/glTexture.h"

Material::Material()
{
}

Material::Material(std::string _vertexShader, std::string _fragmentShader):
	vertexShader(_vertexShader),
	fragmentShader(_fragmentShader),
	vertexShaderCode(""),
	fragmentShaderCode("")
{
}

Material& Material::addTexture(std::string uniform, Texture *texture)
{
	glTexture *glTex = new glTexture(uniform, texture);
	textures.push_front(glTex);

	addUniform(new Uniform(uniform, glTex));

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
	vertexShaderCode = get_shader(vertexShader);

	return vertexShaderCode;
}

const std::string &Material::get_fragmentShader(void)
{
	if (fragmentShaderCode != "") {
		return fragmentShaderCode;
	}
	fragmentShaderCode = get_shader(fragmentShader);

	return fragmentShaderCode;
}

std::string Material::hash(void)
{
	return vertexShader+fragmentShader;
}

std::string Material::get_shader(const std::string shader_file)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string code;
	std::ifstream file;

	// ensure ifstream objects can throw exceptions:
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		file.open(shader_file);
		std::stringstream vShaderStream;
		// read file's buffer contents into streams
		vShaderStream << file.rdbuf();
		// close file handlers
		file.close();
		// convert stream into string
		code = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER " << shader_file << " ::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		exit(-1);
	}

	int hasDefines = code.find("#define DEFINES");
	if (hasDefines >= 0) {
		std::string _defines = defines + "\n#define DEFINES\n";
		code.replace(hasDefines, sizeof("#define DEFINES"), _defines);
	}

	return code;
}

Material::~Material()
{
	std::list <glTexture *> ::iterator it;
	for (it = textures.begin(); it != textures.end(); ++it) {
		delete (*it);
	}
}