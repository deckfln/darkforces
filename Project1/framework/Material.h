#pragma once
#include <iostream>
#include <list>
#include "glTexture.h"
#include "glBufferAttribute.h"
#include "glEngine/glProgram.h"
#include "Uniform.h"
#include "Texture.h"

class Material
{
	std::string vertexShader;
	std::string fragmentShader;

	glProgram *program;

	int current_texture = 0;

	std::list <Uniform *> uniforms;
	std::list <glTexture *> textures;

public:
	Material();
	Material(std::string vertexShader, std::string fragmentShader);
	Material &addTexture(std::string uniform, Texture *texture);
	Material &addShaders(std::string vertexShader, std::string fragmentShader);
	Material &addUniform(Uniform *uniform);
	glProgram &run(void);
	glProgram &get_program(void);
	void bindTextures(void);
	void set_uniforms(void);
	~Material();
};
