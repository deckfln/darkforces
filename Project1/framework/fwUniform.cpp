#include "fwUniform.h"
#include "../glad/glad.h"

#include "../glEngine/glUniformBuffer.h"
#include "../glEngine/glProgram.h"

#include "../glEngine/glTexture.h"
#include "../glEngine/glTextureArray.h"
#include "../glEngine/glCubeTexture.h"

fwUniform::fwUniform()
{
}

fwUniform::fwUniform(const std::string& _name, glm::vec4 *v4):
	m_name(_name)
{
	m_data = v4;
	m_type = GL_FLOAT_VEC4;
}

fwUniform::fwUniform(const std::string& _name, glm::vec3* v3):
	m_name(_name)
{
	m_data = v3;
	m_type = GL_FLOAT_VEC3;
}

fwUniform::fwUniform(const std::string& _name, glm::vec2 *v2) :
	m_name(_name)
{
	m_data = v2;
	m_type = GL_FLOAT_VEC2;
}

fwUniform::fwUniform(const std::string& _name, GLint *id):
	m_name(_name)
{
	m_data = id;
	m_type = GL_INT;
}

fwUniform::fwUniform(const std::string& _name, GLfloat *f) :
	m_name(_name)
{
	m_data = f;
	m_type = GL_FLOAT;
}

fwUniform::fwUniform(const std::string& _name, glTexture *t) :
	m_name(_name)
{
	m_data = t;
	m_type = GL_SAMPLER_2D;
}

fwUniform::fwUniform(const std::string& _name, glTextureArray* t) :
	m_name(_name)
{
	m_data = t;
	m_type = GL_TEXTURE_2D_ARRAY;
}

fwUniform::fwUniform(const std::string& _name, glCubeTexture *t) :
	m_name(_name)
{
	m_data = t;
	m_type = GL_TEXTURE_CUBE_MAP;
}

fwUniform::fwUniform(const std::string& _name, glm::mat4* t, int size) :
	m_name(_name),
	m_data(t),
	m_size(size),
	m_type(GL_FLOAT_MAT4)
{
}

fwUniform::fwUniform(const std::string& _name, glm::vec4* t, int size) :
	m_name(_name+"[0]"),
	m_data(t),
	m_size(size),
	m_type(GL_FLOAT_VEC4)
{
}

/**
 * Create a unifrom for a UBO
 */
fwUniform::fwUniform(const std::string& _name, glUniformBuffer* ubo) :
	m_name(_name),
	m_data(ubo),
	m_type(GL_UBO)
{
}

void *fwUniform::get(void)
{
	return m_data;
}

void fwUniform::set(void *_data)
{
	m_data = _data;
}

/**
 *
 */
void fwUniform::set(const std::string& _name, glm::vec4* _v4)
{
	m_name = _name;
	m_data = _v4;
	m_type = GL_FLOAT_VEC4;
}

void fwUniform::set(const std::string& _name, glm::vec2* _v2)
{
	m_name = _name;
	m_data = _v2;
	m_type = GL_FLOAT_VEC2;
}

void fwUniform::set(const std::string& _name, GLint* id)
{
	m_name = _name;
	m_data = id;
	m_type = GL_INT;
}

void fwUniform::set(const std::string& _name, GLfloat* f)
{
	m_name = _name;
	m_data = f;
	m_type = GL_FLOAT;
}

void fwUniform::set(const std::string& _name, glTexture* t)
{
	m_name = _name;
	m_data = t;
	m_type = GL_SAMPLER_2D;
}

void fwUniform::set(const std::string& _name, glTextureArray* t)
{
	m_name = _name;
	m_data = t;
	m_type = GL_TEXTURE_2D_ARRAY;
}

void fwUniform::set(const std::string& _name, glCubeTexture* t)
{
	m_name = _name;
	m_data = t;
	m_type = GL_TEXTURE_CUBE_MAP;
}

void fwUniform::set(const std::string& _name, int32_t* t, int size)
{
	m_name = _name + "[0]";
	m_data = t;
	m_size = size;
	m_type = GL_INT;
}

void fwUniform::set(const std::string& _name, glm::mat4* t, int size)
{
	m_name = _name;
	m_data = t;
	m_size = size;
	m_type = GL_FLOAT_MAT4;
}

void fwUniform::set(const std::string& _name, glm::vec3* t, int size)
{
	m_name = _name + "[0]";
	m_data = t;
	m_size = size;
	m_type = GL_FLOAT_VEC3;
}

void fwUniform::set(const std::string& _name, glm::vec4* t, int size)
{
	m_name = _name + "[0]";
	m_data = t;
	m_size = size;
	m_type = GL_FLOAT_VEC4;
}

void fwUniform::set(const std::string& _name, glUniformBuffer* ubo)
{
	m_name = _name;
}

/**
 *
 */
void fwUniform::set_uniform(glProgram *program)
{
	// ignore non initalizerd uniforms
	if (m_data == nullptr) {
		return;
	}

	switch (m_type) {
	case GL_FLOAT:
		program->set_uniform(m_name, *(GLfloat *)m_data);
		break;
	case GL_INT:
		program->set_uniform(m_name, *(GLint*)m_data);
		break;
	case GL_FLOAT_VEC4:
		// TODO : for single vec4 it should be size 1
		if (m_size == 0) {
			program->set_uniform(m_name, *(glm::vec4*)m_data);
		}
		else {
			program->set_uniform(m_name, (glm::vec4*)m_data, m_size);
		}
		break;
	case GL_FLOAT_VEC2:
		program->set_uniform(m_name, *(glm::vec2 *)m_data);
		break;
	case GL_FLOAT_VEC3:
		program->set_uniform(m_name, *(glm::vec3*)m_data);
		break;
	case GL_SAMPLER_2D:
		program->set_uniform(m_name, (glTexture *)m_data);
		break;
	case GL_TEXTURE_2D_ARRAY:
		program->set_uniform(m_name, (glTexture *)m_data);
		break;
	case GL_TEXTURE_CUBE_MAP:
		program->set_uniform(m_name, (glTexture *)m_data);
		break;
	case GL_FLOAT_MAT4:
		program->set_uniform(m_name, (glm::mat4 *)m_data, m_size);
		break;
	case GL_UBO:
		((glUniformBuffer *)m_data)->bind(program, m_name);
		break;
	}
}
