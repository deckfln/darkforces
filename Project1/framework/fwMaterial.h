#pragma once
#include <iostream>
#include <list>

#include "../Reference.h"
#include "../glEngine/glProgram.h"
#include "fwUniform.h"
#include "fwTexture.h"

class fwMaterial: public Reference
{
	int id;
	std::string vertexShader;
	std::string fragmentShader;
	std::string geometryShader;
	std::string vertexShaderCode;
	std::string fragmentShaderCode;
	std::string geometryShaderCode;

	int current_texture = 0;

	std::list <fwUniform *> uniforms;
	std::list <glTexture *> textures;

protected:
	std::string defines;

public:
	fwMaterial();
	fwMaterial(std::string vertexShader, std::string fragmentShader, std::string geometryShader);
	fwMaterial &addTexture(std::string uniform, fwTexture *texture);
	fwMaterial &addTexture(std::string uniform, glTexture *texture);
	fwMaterial &addShaders(std::string vertexShader, std::string fragmentShader, const std::string defines = "");
	fwMaterial &addUniform(fwUniform *uniform);
	
	std::string hashCode(void);

	const std::string &get_vertexShader(void);
	const std::string &get_fragmentShader(void);
	const std::string &get_geometryShader(void);
	const int getID(void);

	void bindTextures(void);
	void set_uniforms(glProgram *program);
	~fwMaterial();
};
