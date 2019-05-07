#pragma once
#include <iostream>
#include <list>
#include "glEngine/glProgram.h"
#include "Uniform.h"
#include "Texture.h"

class Material
{
	std::string vertexShader;
	std::string fragmentShader;

	int current_texture = 0;

	std::list <Uniform *> uniforms;
	std::list <glTexture *> textures;

public:
	Material();
	Material(std::string vertexShader, std::string fragmentShader);
	Material &addTexture(std::string uniform, Texture *texture);
	Material &addShaders(std::string vertexShader, std::string fragmentShader);
	Material &addUniform(Uniform *uniform);
	std::string hash(void);
	const std::string &get_vertexShader(void);
	const std::string &get_fragmentShader(void);

	void bindTextures(void);
	void set_uniforms(glProgram *program);
	~Material();
};
