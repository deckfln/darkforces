#include "fwMaterial.h"
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "../glEngine/glTexture.h"
#include "../glEngine/glTextureArray.h"
#include "../glEngine/glProgram.h"
#include "../glEngine/glVertexArray.h"

#include "fwGeometry.h"
#include "fwUniform.h"
#include "fwTexture.h"
#include "fwTextures.h"

static int g_materialID = 0;

fwMaterial::fwMaterial():
	m_id(g_materialID++)
{
}

fwMaterial::fwMaterial(const std::string& _vertexShader, const std::string& _fragmentShader, const std::string& _geometryShader):
	m_id(g_materialID++)
{
	addShader(VERTEX_SHADER, _vertexShader);
	if (_geometryShader != "") {
		addShader(GEOMETRY_SHADER, _geometryShader);
	}
	addShader(FRAGMENT_SHADER, _fragmentShader);
}

/**
 * Create a Material with 1 argument
 */
fwMaterial::fwMaterial(const std::map<ShaderType, std::string>& shaders):
	m_id(g_materialID++)
{
	for (auto& shader : shaders) {
		if (shader.first == FRAGMENT_SHADER_DEFERED) {
			addShader(FRAGMENT_SHADER, shader.second, DEFERED_RENDER);
		}
		else {
			addShader(shader.first, shader.second);
		}
	}
}

fwMaterial::fwMaterial(const std::map<ShaderType, std::string>* pShaders) :
	m_id(g_materialID++)
{
	for (auto& shader : *pShaders) {
		if (shader.first == FRAGMENT_SHADER_DEFERED) {
			addShader(FRAGMENT_SHADER, shader.second, DEFERED_RENDER);
		}
		else {
			addShader(shader.first, shader.second);
		}
	}
}

/**
 * Add a single hiegh level texture
 */
fwMaterial& fwMaterial::addTexture(const std::string& uniform, fwTexture *texture)
{
	glTexture* glTex = nullptr;
	if (texture != nullptr) {
		glTex = new glTexture(texture);
		m_textures[texture->id()] = glTex;
	}

	addUniform(new fwUniform(uniform, glTex));

	return *this;
}

/**
 * Add a single low level texture
 */
fwMaterial& fwMaterial::addTexture(const std::string& uniform, glTexture *texture)
{
	// m_textures[texture->getID()] = texture;

	addUniform(new fwUniform(uniform, texture));

	return *this;
}

/**
 * Add a high level multi-texture
 */
fwMaterial& fwMaterial::addTextures(const std::string& uniform, fwTextures* textures)
{
	glTextureArray* glTex = new glTextureArray(textures);
	m_textureArrays.push_front(glTex);

	addUniform(new fwUniform(uniform, glTex));

	return *this;
}

/**
 * Add a generic uniform
 */
fwMaterial &fwMaterial::addUniform(fwUniform *uniform)
{
	m_uniforms[uniform->name()] = uniform;
	return *this;
}

/**
 * add a variable to the material
 */
void fwMaterial::addVariable(const std::string& variable, glm::vec2& v2)
{
	if (m_uniforms.count(variable) == 0) {
		m_uniforms[variable] = new fwUniform(variable, &v2);
	}
}

void fwMaterial::addVariable(const std::string& variable, glm::vec3& v3)
{
	if (m_uniforms.count(variable) == 0) {
		m_uniforms[variable] = new fwUniform(variable, &v3);
	}
}

void fwMaterial::addVariable(const std::string& variable, glm::vec4& v4)
{
	if (m_uniforms.count(variable) == 0) {
		m_uniforms[variable] = new fwUniform(variable, &v4);
	}
}

void fwMaterial::addVariable(const std::string& variable, float& f)
{
	if (m_uniforms.count(variable) == 0) {
		m_uniforms[variable] = new fwUniform(variable, &f);
	}
}

void fwMaterial::addVariable(const std::string& variable, int32_t& i)
{
	if (m_uniforms.count(variable) == 0) {
		m_uniforms[variable] = new fwUniform(variable, &i);
	}
}

/**
 * Activate uniforms on the given program
 */
void fwMaterial::set_uniforms(glProgram *program)
{
	for (auto& uniform: m_uniforms) {
		if (uniform.second != nullptr) {
			uniform.second->set_uniform(program);
		}
	}
}

/**
 * set a uniform texture
 */
void fwMaterial::set(const std::string& name, fwTexture* texture)
{
	glTexture* glTex=nullptr;

	if (texture == nullptr) {
		return;
	}

	// create the glTexture the first time
	if (m_textures.count(texture->id()) == 0) {
		m_textures[texture->id()] = new glTexture(texture);
	}

	// update the glTexture if the fwTexture is dirty
	if (texture->isDirty()) {
		m_textures[texture->id()]->update(texture);
		texture->dirty(false);
	}
	fwUniform* uniform = m_uniforms[name];
	if (uniform) {
		// setting a uniform that exists
		uniform->set(m_textures[texture->id()]);
	}
}

/**
 * set a uniform texture
 */
void fwMaterial::set(const std::string& name, glm::vec4* v)
{
	fwUniform* uniform = m_uniforms[name];
	if (uniform) {
		// setting a uniform that exists
		uniform->set(v);
	}
}

/**
 *
 */
fwMaterial* fwMaterial::clone(void)
{
	fwMaterial* material = new fwMaterial();
	material->m_files = m_files;
	material->m_shaders = m_shaders;
	material->m_currentTexture = m_currentTexture;
	material->m_uniforms = m_uniforms;
	material->m_textures = m_textures;
	material->m_textureArrays = m_textureArrays;
	material->m_hash = m_hash;
	material->m_type = m_type;
	material->m_defines = m_defines;

	return material;
}

/**
 * Bind all textures
 */
void fwMaterial::bindTextures(void)
{
	for (auto& texture: m_textures) {
		texture.second->bind();
	}
}

/**
 * Load the shader file
 */
std::string fwMaterial::load_shader(int renderer, int shader, const std::string& define)
{
	return load_shader_file(m_files[renderer][shader], "");
}

const std::string &fwMaterial::get_vertexShader(void)
{
	return get_shader(VERTEX_SHADER);
}

const std::string& fwMaterial::get_geometryShader(void)
{
	return get_shader(GEOMETRY_SHADER);
}

const std::string& fwMaterial::get_fragmentShader(void)
{
	return get_shader(FRAGMENT_SHADER);
}

fwMaterial &fwMaterial::addShader(int shader, const std::string& file, RenderType render)
{
	m_files[render][shader] = file; 
	m_shaders[render][shader] = "";

	m_hash += file;

	return *this;
}

const std::string& fwMaterial::get_shader(int shader, RenderType render)
{
	//TODO: return a reference to the real code instead of a copy of the string 
	if (m_shaders[render][shader] == "") {
		std::string& file = m_files[render][shader];
		if (file == "") {
			return m_files[render][shader];
		}
		m_shaders[render][shader] = load_shader_file(file, m_defines);
	}

	return m_shaders[render][shader];
}

const std::string& fwMaterial::get_shader(int shader, RenderType render, std::map<std::string, std::string>& variables)
{
	//TODO: return a reference to the real code instead of a copy of the string 
	if (m_shaders[render][shader] == "") {
		std::string& file = m_files[render][shader];
		if (file == "") {
			return m_files[render][shader];
		}
		m_shaders[render][shader] = load_shader_file(file, m_defines, &variables);
	}

	return m_shaders[render][shader];
}

std::string fwMaterial::hashCode(void)
{
	return m_hash + m_defines;
}

fwMaterial::~fwMaterial()
{
	for (auto& texture: m_textures) {
		delete texture.second;
	}

	for (auto texture : m_textureArrays) {
		delete texture;
	}

	for (auto& uniform : m_uniforms) {
		delete uniform.second;
	}
}