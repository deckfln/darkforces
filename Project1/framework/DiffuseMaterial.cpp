#include "DiffuseMaterial.h"



DiffuseMaterial::DiffuseMaterial():
	diffuse(nullptr),
	specular(nullptr),
	shininess(0)
{
}

DiffuseMaterial::DiffuseMaterial(Texture *_diffuse, Texture *_specular, float _shininess):
	diffuse(_diffuse),
	specular(_specular),
	shininess(_shininess)
{
	if (_specular != nullptr) {
		addShaders("shaders/vertex_diffuse.glsl", "shaders/fragment_diffuse.glsl", "#define SPECULAR_MAP");
		addTexture("material.specular", specular);
	}
	else {
		addShaders("shaders/vertex_diffuse.glsl", "shaders/fragment_diffuse.glsl");
	}
	addTexture("material.diffuse", diffuse);

	uniform = new Uniform("material.shininess", &shininess);
	addUniform(uniform);
}

DiffuseMaterial::~DiffuseMaterial()
{
}
