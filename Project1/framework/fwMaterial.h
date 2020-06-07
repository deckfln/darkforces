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
enum ShaderType {VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER, FRAGMENT_SHADER_DEFERED};

const int MAX_SHADERS = 10;

class glProgram;
class fwGeometry;
class glVertexArray;

class fwMaterial: public Reference
{
	int m_id = 0;

	std::map <int, std::map <int, std::string>> m_files;
	std::map <int, std::map <int, std::string>> m_shaders;

	int m_currentTexture = 0;

	std::map <std::string, fwUniform *> m_uniforms;
	std::map <int, glTexture *> m_textures;
	std::list <glTextureArray *> m_textureArrays;

	std::string m_hash = "";

	glProgram* m_program = nullptr;	// For self executed material
	std::map<int, glVertexArray*> m_vertexArays;

protected:
	int m_type = 0;
	std::string m_defines = "";

public:
	fwMaterial();
	int type(int flag) { return m_type & flag; };
	fwMaterial(std::string vertexShader, std::string fragmentShader, std::string geometryShader = "");
	fwMaterial(std::map<ShaderType, std::string>& shaders);
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
	const std::string &get_shader(int shader, RenderType render = FORWARD_RENDER);
	const std::string &get_shader(int shader, RenderType render, std::map<std::string, std::string> defines);
	const int id(void) { return m_id; };
	const std::string defines(void) { return m_defines; };

	void bindTextures(void);
	void set_uniforms(glProgram *program);
	void set(const std::string& name, fwTexture* texture);
	void set(const std::string& name, glm::vec4* v4);

	void draw(fwGeometry *geometry);	// self-executer material

	~fwMaterial();
};
