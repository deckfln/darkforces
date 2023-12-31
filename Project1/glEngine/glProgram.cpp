#include "glProgram.h"

#include <iostream>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "../config.h"
#include "glShader.h"

#include "../List.h"
#include "../glad/glad.h"

#include "glUniformBlock.h"

List Shaders;

std::string load_shader_file(const std::string& shader_file, 
	const std::string& defines, 
	std::map <std::string, std::string> *variables)
{
	static std::string empty="";

	if (shader_file == "") {
		return empty;
	}

	// 1. retrieve the vertex/fragment source code from filePath
	std::string code;
	std::ifstream file;

	// ensure ifstream objects can throw exceptions:
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		file.open(ROOT_FOLDER + shader_file);
		std::stringstream vShaderStream;
		// read file's buffer contents into streams
		vShaderStream << file.rdbuf();
		// close file handlers
		file.close();
		// convert stream into string
		code = vShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "glProgram::load_shader_file " << ROOT_FOLDER + shader_file << " ::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		exit(-1);
	}

	// replace variables
	if (variables != nullptr) {
		int pos = -1;
		for (auto m : *variables) {
			std::string v = "#" + m.first;
			pos = code.find(v);
			if (pos >= 0) {
				code.replace(pos, v.length(), m.second);
			}
		}
	}

	// deal with all includes
	const std::regex re_basename("(.*)/");
	std::smatch base_match;
	std::string path = "";

	if (std::regex_search(shader_file, base_match, re_basename)) {
		path = base_match[1].str();
	}

	const std::regex re("#include \"([^\"]*)\"");

	int hasInclude = 1;
	while ((hasInclude = code.find("#include")) >= 0) {
		if (std::regex_search(code, base_match, re)) {
			// The first sub_match is the whole string; the next
			// sub_match is the first parenthesized expression.
			if (base_match.size() == 2) {
				std::string line = base_match[0].str();
				std::string file = base_match[1].str();

				std::string include;
				if (file[0] == '/') {
					// absolute file
					include = load_shader_file(file.substr(1), defines);
				}
				else {
					// relative file
					include = load_shader_file(path + "/" + file, defines);
				}

				code.replace(hasInclude, line.length(), include);
			}
		}
	}

	// deal with provided defines

	int hasDefines = code.find("#define DEFINES");
	if (hasDefines >= 0) {
		std::string _defines = defines + "\n#define DEFINES\n";
		code.replace(hasDefines, sizeof("#define DEFINES"), _defines);
	}

	return code;
}

glProgram::glProgram(void)
{
}

glProgram::glProgram(const std::string& vertexShader, 
	const std::string& fragmentShader, 
	const std::string& geometryShader, 
	const std::string& defines)
{
	glShader *vertex = new glShader(vertexShader, defines, GL_VERTEX_SHADER);
	glShader *fragment = new glShader(fragmentShader, defines, GL_FRAGMENT_SHADER);
	glShader *geometry = nullptr;
	if (geometryShader != "") {
		geometry = new glShader(geometryShader, defines, GL_GEOMETRY_SHADER);
	}
	// link shaders
	m_id = glCreateProgram();

	glAttachShader(m_id, vertex->id);
	glAttachShader(m_id, fragment->id);
	if (geometry) {
		glAttachShader(m_id, geometry->id);
	}
	glLinkProgram(m_id);

	// check for linking errors
	int success;
	char infoLog[512];
	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		exit(-1);
	}

	// extract active attributes
	GLint n;
	GLchar name[255];
	GLsizei length;
	GLsizei size;
	GLenum type;
	GLint location;

	glGetProgramiv(m_id, GL_ACTIVE_ATTRIBUTES, &n);
	for (int i = 0; i < n; i++) {
		glGetActiveAttrib(m_id, i, sizeof(name), &length, &size, &type, name);
		location = glGetAttribLocation(m_id, name);

		glVertexAttribute *attr = new glVertexAttribute(name, length, size, type, location);
		attributes[name] = attr;
	}

	// extract active uniforms
	glGetProgramiv(m_id, GL_ACTIVE_UNIFORMS, &n);
	for (int i = 0; i < n; i++) {
		glGetActiveUniform(m_id, i, sizeof(name), &length, &size, &type, name);
		location = glGetUniformLocation(m_id, name);

		glUniform *uniform = new glUniform(name, length, size, type, location);
		uniforms[name] = uniform;
	}

	// extract active uniform blocks
	glGetProgramInterfaceiv(m_id, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &m_nbUniformBlocks);

	for (auto i = 0; i < m_nbUniformBlocks; i++) {
		glGetProgramResourceName(m_id, GL_UNIFORM_BLOCK, i, sizeof(name), &length, name);
		if (m_uniformBlocks.count(name) == 0) {
			m_uniformBlocks[name] = new glUniformBlock(name, m_id, i);
		}
	}

	delete vertex;
	delete fragment;
	if (geometry) {
		delete geometry;
	}
}

GLuint glProgram::getID(void)
{
	return m_id;
}

void glProgram::run(void)
{
	glUseProgram(m_id);
}

glVertexAttribute *glProgram::get_attribute(const std::string& name)
{
	glVertexAttribute *attr = attributes[name];
	if (!attr) {
		//std::cout << "glProgram::get_attribute " << name.c_str() << " mising" << std::endl;
		return NULL;
	}
	return attr;
}

bool glProgram::bindBufferAttribute(const std::string& name, int bindingPoint)
{
	if (uniformBufferBindingPoints.count(name) == 0) {
		GLuint blockIndex = glGetUniformBlockIndex(m_id, name.c_str());

		if (blockIndex >= 0) {
			glUniformBlockBinding(m_id, blockIndex, bindingPoint);
		}

		uniformBufferBindingPoints[name] = blockIndex;
		return true;
	}

	return false;
}

glUniform *glProgram::get_uniform(const std::string& name)
{
	glUniform *attr = uniforms[name];
	if (!attr) {
		//std::cout << "glProgram::get_uniform " << name.c_str() << " mising" << std::endl;
	}
	return attr;
}

void glProgram::set_uniform(const std::string& name, GLint id)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(id);
}

void glProgram::set_uniform(const std::string& name, GLfloat f)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(f);
}

void glProgram::set_uniform(const std::string& name, GLfloat r, GLfloat g, GLfloat b)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(r, g, b);
}

void glProgram::set_uniform(const std::string& name, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(r, g, b, a);
}

void glProgram::set_uniform(const std::string& name, const glm::mat4 &mat4)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(mat4);
}

void glProgram::set_uniform(const std::string& name, const glm::vec4 &vec4)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(vec4);
}

void glProgram::set_uniform(const std::string& name, glm::vec4* vec4, int size)
{
	glUniform* uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(vec4, size);
}

void glProgram::set_uniform(const std::string& name, const glm::vec3 &vec3)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(vec3);
}

void glProgram::set_uniform(const std::string& name, const glm::vec2 &vec2)
{
	glUniform *uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(vec2);
}

void glProgram::set_uniform(const std::string& name, glm::mat4 *mat4, int size)
{
	glUniform* uniform = get_uniform(name);
	if (uniform)
		uniform->set_value(mat4, size);
}

void glProgram::set_uniform(const std::string& name, glTexture *texture)
{
	glUniform *uniform = get_uniform(name);
	if (uniform) {
		int textureUnit = texture->bind();
		// std::cout << "glProgram::set_uniform " << name.c_str() << " : " << id << std::endl;
		if (textureUnit != -1) {
			// texture was not yet bound to texture unit, need to upload to the uniform
			uniform->set_value(textureUnit);
		}
	}
}

glProgram::~glProgram()
{
	glDeleteProgram(m_id);

	for (auto attribute : attributes) {
		delete attribute.second;

	}
	for (auto uniform : uniforms) {
		delete uniform.second;
	}

	for (auto block : m_uniformBlocks) {
		delete block.second;
	}
}
