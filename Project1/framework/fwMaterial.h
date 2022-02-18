#pragma once
#include <iostream>
#include <list>
#include <map>
#include <glm/vec4.hpp>

#include "../Reference.h"

enum RenderType {FORWARD_RENDER, DEFERED_RENDER};
enum ShaderType {VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER, FRAGMENT_SHADER_DEFERED};

const int MAX_SHADERS = 10;

class fwUniform;
class glTexture;
class glTextureArray;
class glProgram;
class fwTexture;
class fwTextures;

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

protected:
	int m_type = 0;
	std::string m_defines = "";

public:
	fwMaterial();
	int type(int flag) { return m_type & flag; };
	fwMaterial(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader = "");
	fwMaterial(const std::map<ShaderType, std::string>& shaders);
	fwMaterial(const std::map<ShaderType, std::string>* pShaders);
	fwMaterial &addTexture(const std::string& uniform, fwTexture *texture);
	fwMaterial &addTexture(const std::string& uniform, glTexture *texture);
	fwMaterial& addTextures(const std::string& uniform, fwTextures* textures);

	// fwMaterial &addShaders(std::string vertexShader, std::string fragmentShader, const std::string defines = "");
	fwMaterial &addUniform(fwUniform *uniform);
	fwMaterial &addShader(int shader, const std::string& file, RenderType render = FORWARD_RENDER);

	std::string hashCode(void);

	std::string load_shader(int renderer, int shader, const std::string& define);
	const std::string &get_vertexShader(void);
	const std::string &get_fragmentShader(void);
	const std::string &get_geometryShader(void);
	const std::string &get_shader(int shader, RenderType render = FORWARD_RENDER);
	const std::string &get_shader(int shader, RenderType render, std::map<std::string, std::string>& defines);
	const int id(void) { return m_id; };
	const std::string defines(void) { return m_defines; };

	void bindTextures(void);
	void set_uniforms(glProgram *program);
	void set(const std::string& name, fwTexture* texture);
	void set(const std::string& name, glm::vec4* v4);

	~fwMaterial();
};
