#pragma once
#include <iostream>
#include <list>

#include "../Reference.h"
#include "../glEngine/glProgram.h"
#include "Uniform.h"
#include "Texture.h"

class Material: public Reference
{
	int id;
	std::string vertexShader;
	std::string fragmentShader;
	std::string defines;

	std::string vertexShaderCode;
	std::string fragmentShaderCode;

	int current_texture = 0;

	std::list <Uniform *> uniforms;
	std::list <glTexture *> textures;

	std::string get_shader(const std::string shader_file);

public:
	Material();
	Material(std::string vertexShader, std::string fragmentShader);
	Material &addTexture(std::string uniform, Texture *texture);
	Material &addShaders(std::string vertexShader, std::string fragmentShader, const std::string defines = "");
	Material &addUniform(Uniform *uniform);
	
	std::string hashCode(void);

	const std::string &get_vertexShader(void);
	const std::string &get_fragmentShader(void);
	const int getID(void);

	void bindTextures(void);
	void set_uniforms(glProgram *program);
	~Material();
};
