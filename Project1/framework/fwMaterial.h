#pragma once
#include <iostream>
#include <list>
#include <map>
#include "../Reference.h"
#include "../glEngine/glProgram.h"
#include "fwUniform.h"
#include "fwTexture.h"
#include "fwTextures.h"

enum RenderType {FORWARD_RENDER, DEFERED_RENDER};
enum ShaderType {VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER};

const int MAX_SHADERS = 10;

class fwMaterial: public Reference
{
	int id;
	/*
	std::string vertexShader = "";
	std::string fragmentShader = "";
	std::string geometryShader = "";
	*/
	std::string vertexShaderCode = "";
	std::string fragmentShaderCode = "";
	std::string geometryShaderCode = "";

	std::map <int, std::map <int, std::string>> files;
	std::map <int, std::map <int, std::string>> shaders;

	int current_texture = 0;

	std::list <fwUniform *> uniforms;
	std::list <glTexture *> textures;
	std::list <glTextureArray *> m_textureArrays;

	std::string m_hash = "";

protected:
	int m_type = 0;
	std::string m_defines = "";

public:
	fwMaterial();
	int type(int flag) { return m_type & flag; };
	fwMaterial(std::string vertexShader, std::string fragmentShader, std::string geometryShader = "");
	fwMaterial &addTexture(std::string uniform, fwTexture *texture);
	fwMaterial &addTexture(std::string uniform, glTexture *texture);
	fwMaterial& addTextures(std::string uniform, fwTextures* textures);

	// fwMaterial &addShaders(std::string vertexShader, std::string fragmentShader, const std::string defines = "");
	fwMaterial &addUniform(fwUniform *uniform);
	fwMaterial &addShader(int shader, std::string file, RenderType render = FORWARD_RENDER);

	std::string hashCode(void);

	const std::string &get_vertexShader(void);
	const std::string &get_fragmentShader(void);
	const std::string &get_geometryShader(void);
	const std::string get_shader(int shader, RenderType render = FORWARD_RENDER);
	const int getID(void);
	const std::string defines(void) { return m_defines; };

	void bindTextures(void);
	void set_uniforms(glProgram *program);
	~fwMaterial();
};
