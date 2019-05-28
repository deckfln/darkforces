#include "fwUniform.h"
#include "glad/glad.h"


fwUniform::fwUniform()
{
}

fwUniform::fwUniform(std::string _name, glm::vec4 *v4):
	name(_name)
{
	data = v4;
	type = GL_FLOAT_VEC4;
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

fwUniform::fwUniform(std::string _name, glCubeTexture *t) :
	name(_name)
{
	data = t;
	type = GL_TEXTURE_CUBE_MAP;
}

void *fwUniform::get_value(void)
{
	return data;
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
	case GL_SAMPLER_2D:
		program->set_uniform(name, (glTexture *)data);
		break;
	case GL_TEXTURE_CUBE_MAP:
		program->set_uniform(name, ((glCubeTexture *)data)->get_textureUnit());
		break;
	}
}

fwUniform::~fwUniform()
{
}
