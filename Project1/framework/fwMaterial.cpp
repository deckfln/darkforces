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

static int g_materialID = 0;

fwMaterial::fwMaterial():
	m_id(g_materialID++)
{
}

fwMaterial::fwMaterial(std::string _vertexShader, std::string _fragmentShader, std::string _geometryShader):
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
fwMaterial::fwMaterial(std::map<ShaderType, std::string>& shaders):
	m_id(g_materialID++)
{
	for (auto shader : shaders) {
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
fwMaterial& fwMaterial::addTexture(std::string uniform, fwTexture *texture)
{
	glTexture *glTex = new glTexture(texture);
	m_textures[texture->id()] = glTex;

	addUniform(new fwUniform(uniform, glTex));

	return *this;
}

/**
 * Add a single low level texture
 */
fwMaterial& fwMaterial::addTexture(std::string uniform, glTexture *texture)
{
	// m_textures[texture->getID()] = texture;

	addUniform(new fwUniform(uniform, texture));

	return *this;
}

/**
 * Add a high level multi-texture
 */
fwMaterial& fwMaterial::addTextures(std::string uniform, fwTextures* textures)
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
 * Activate uniforms on the given program
 */
void fwMaterial::set_uniforms(glProgram *program)
{
	for (auto uniform: m_uniforms) {
		uniform.second->set_uniform(program);
	}
}

/**
 * set a uniform texture
 */
void fwMaterial::set(const std::string& name, fwTexture* texture)
{
	glTexture* glTex=nullptr;

	if (m_textures.count(texture->id()) == 0) {
		m_textures[texture->id()] = new glTexture(texture);
	}

	m_uniforms[name]->set(m_textures[texture->id()]);
}

/**
 * set a uniform texture
 */
void fwMaterial::set(const std::string& name, glm::vec4* v)
{
	m_uniforms[name]->set(v);
}

/**
 * Run a self-executed material (not part of the rendering process)
 */
void fwMaterial::draw(fwGeometry* geometry)
{
	if (m_program == nullptr) {
		std::string vs = load_shader_file(m_files[FORWARD_RENDER][VERTEX_SHADER], "");
		std::string fs = load_shader_file(m_files[FORWARD_RENDER][FRAGMENT_SHADER], "");
		std::string gs = load_shader_file(m_files[FORWARD_RENDER][GEOMETRY_SHADER], "");
		m_program = new glProgram(vs, fs, gs, "");
	}
	m_program->run();

	if (m_vertexArays.count(geometry->id()) == 0) {
		m_vertexArays[geometry->id()] = new glVertexArray();
		geometry->enable_attributes(m_program);
		m_vertexArays[geometry->id()]->unbind();
	}

	set_uniforms(m_program);
	geometry->draw(GL_TRIANGLES, m_vertexArays[geometry->id()]);
}

/**
 * Bind all textures
 */
void fwMaterial::bindTextures(void)
{
	for (auto texture: m_textures) {
		texture.second->bind();
	}
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

fwMaterial &fwMaterial::addShader(int shader, std::string file, RenderType render)
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

const std::string& fwMaterial::get_shader(int shader, RenderType render, std::map<std::string, std::string>variables)
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
	for (auto texture: m_textures) {
		delete texture.second;
	}

	for (auto texture : m_textureArrays) {
		delete texture;
	}

	for (auto uniform : m_uniforms) {
		delete uniform.second;
	}
}