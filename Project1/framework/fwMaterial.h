#pragma once
#include <iostream>
#include <list>
#include <map>
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
	std::map <const std::string, std::string> shaders;
	std::map <const std::string, std::string> shaderCode;

	int current_texture = 0;

	std::list <fwUniform *> uniforms;
	std::list <glTexture *> textures;

protected:
	int m_type = 0;
	std::string m_defines;

public:
	fwMaterial();
	int type(int flag) { return m_type & flag; };
	fwMaterial(std::string vertexShader, std::string fragmentShader, std::string geometryShader);
	fwMaterial &addTexture(std::string uniform, fwTexture *texture);
	fwMaterial &addTexture(std::string uniform, glTexture *texture);
	fwMaterial &addShaders(std::string vertexShader, std::string fragmentShader, const std::string defines = "");
	fwMaterial &addUniform(fwUniform *uniform);
	fwMaterial &addShader(const std::string name, std::string file);

	std::string hashCode(void);

	const std::string &get_vertexShader(void);
	const std::string &get_fragmentShader(void);
	const std::string &get_geometryShader(void);
	const std::string &get_shader(const std::string name);
	const int getID(void);
	const std::string defines(void) { return m_defines; };

	void bindTextures(void);
	void set_uniforms(glProgram *program);
	~fwMaterial();
};
