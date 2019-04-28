#pragma once
#include "glTexture.h"
#include "glBufferAttribute.h"

class Material
{
	int max_textures = 10;
	int current_texture = 0;
	glTexture *textures[10];

public:
	Material();
	Material &addTexture(std::string uniform, std::string file_name);
	void bindTextures(glProgram &program);
	~Material();
};

