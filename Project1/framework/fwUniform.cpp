#include "fwUniform.h"
#include "../glad/glad.h"


fwUniform::fwUniform()
{
}

fwUniform::fwUniform(std::string _name, glm::vec4 *v4):
	name(_name)
{
	data = v4;
	type = GL_FLOAT_VEC4;
}

fwUniform::fwUniform(std::string _name, glm::vec2 *v2) :
	name(_name)
{
	data = v2;
	type = GL_FLOAT_VEC2;
}

fwUniform::fwUniform(std::string _name, GLint *id):
	name(_name)
{
	data = id;
	type = GL_SAMPLER_2D;
}

fwUniform::fwUniform(std::string _name, GLfloat *f) :
	name(_name)
{
	data = f;
	type = GL_FLOAT;
}

fwUniform::fwUniform(std::string _name, glTexture *t) :
	name(_name)
{
	data = t;
	type = GL_SAMPLER_2D;
}

fwUniform::fwUniform(std::string _name, glTextureArray* t) :
	name(_name)
{
	data = t;
	type = GL_TEXTURE_2D_ARRAY;
}

fwUniform::fwUniform(std::string _name, glCubeTexture *t) :
	name(_name)
{
	data = t;
	type = GL_TEXTURE_CUBE_MAP;
}

fwUniform::fwUniform(std::string _name, glm::mat4* t, int size) :
	name(_name),
	data(t),
	m_size(size),
	type(GL_FLOAT_MAT4)
{
}

void *fwUniform::get_value(void)
{
	return data;
}

void fwUniform::set(void *_data)
{
	data = _data;
}

void fwUniform::set_uniform(glProgram *program)
{
	switch (type) {
	case GL_FLOAT:
		program->set_uniform(name, *(GLfloat *)data);
		break;
	case GL_FLOAT_VEC4:
		program->set_uniform(name, *(glm::vec4 *)data);
		break;
	case GL_FLOAT_VEC2:
		program->set_uniform(name, *(glm::vec2 *)data);
		break;
	case GL_SAMPLER_2D:
		program->set_uniform(name, (glTexture *)data);
		break;
	case GL_TEXTURE_2D_ARRAY:
		program->set_uniform(name, (glTexture*)data);
		break;
	case GL_TEXTURE_CUBE_MAP:
		program->set_uniform(name, ((glCubeTexture *)data)->get_textureUnit());
		break;
	case GL_FLOAT_MAT4:
		program->set_uniform(name, (glm::mat4 *)data, m_size);
		break;
	}
}

fwUniform::~fwUniform()
{
}
