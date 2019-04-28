#include "Material.h"
#include "glBufferAttribute.h"


Material::Material()
{
}

Material& Material::addTexture(std::string uniform, std::string file_name)
{
	textures[current_texture] = new glTexture(uniform, file_name);
	current_texture++;

	return *this;
}

void Material::bindTextures(glProgram &program)
{
	for (int i = 0; i < current_texture; i++) {
		textures[i]->bind(program);
	}
}

Material::~Material()
{
	for (int i = 0; i < current_texture; i++) {
		delete textures[i];
	}
}