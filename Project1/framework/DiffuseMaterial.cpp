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
	shininess(_shininess),
	uniform("material.shininess", &shininess)
{
	addShaders("shaders/vertex_diffuse.glsl", "shaders/fragment_diffuse.glsl");
	addTexture("material.diffuse", diffuse);
	addTexture("material.specular", specular);

	addUniform(&uniform);
}

DiffuseMaterial::~DiffuseMaterial()
{
}
