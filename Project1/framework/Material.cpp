#include "Material.h"
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

void Material::set_uniforms(void)
{
	std::list <Uniform *> ::iterator it;
	for (it = uniforms.begin(); it != uniforms.end(); ++it) {
		(*it)->set_uniform(*program);
	}
}

void Material::bindTextures(void)
{
	std::list <glTexture *> ::iterator it;
	for (it = textures.begin(); it != textures.end(); ++it) {
		(*it)->bind();
	}
}

glProgram &Material::run(void)
{
	if (program == nullptr) {
		program = new glProgram(vertexShader, fragmentShader);
	}

	program->run();

	return *program;
}

glProgram &Material::get_program(void)
{
	return *program;
}

Material::~Material()
{
	std::list <glTexture *> ::iterator it;
	for (it = textures.begin(); it != textures.end(); ++it) {
		delete (*it);
	}
}