#include "Material.h"
#include <string>
#include <functional>
#include "glEngine/glTexture.h"

Material::Material()
{
}

Material::Material(std::string _vertexShader, std::string _fragmentShader):
	vertexShader(_vertexShader),
	fragmentShader(_fragmentShader)
{
}

Material& Material::addTexture(std::string uniform, Texture *texture)
{
	glTexture *glTex = new glTexture(uniform, texture);
	textures.push_front(glTex);

	addUniform(new Uniform(uniform, glTex));

	return *this;
}

Material &Material::addShaders(std::string _vertexShader, std::string _fragmentShader)
{
	vertexShader = _vertexShader;
	fragmentShader = _fragmentShader;

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
	return vertexShader;
}

const std::string &Material::get_fragmentShader(void)
{
	return fragmentShader;
}

std::string Material::hash(void)
{
	return vertexShader+fragmentShader;
}

Material::~Material()
{
	std::list <glTexture *> ::iterator it;
	for (it = textures.begin(); it != textures.end(); ++it) {
		delete (*it);
	}
}