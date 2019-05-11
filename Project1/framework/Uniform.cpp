#include "Uniform.h"
#include "glad/glad.h"


Uniform::Uniform()
{
}

Uniform::Uniform(std::string _name, glm::vec4 *v4):
	name(_name)
{
	data = v4;
	type = GL_FLOAT_VEC4;
}

Uniform::Uniform(std::string _name, GLint *id):
	name(_name)
{
	data = id;
	type = GL_SAMPLER_2D;
}

Uniform::Uniform(std::string _name, GLfloat *f) :
	name(_name)
{
	data = f;
	type = GL_FLOAT;
}

Uniform::Uniform(std::string _name, glTexture *t) :
	name(_name)
{
	data = t;
	type = GL_SAMPLER_2D;
}

void Uniform::set_uniform(glProgram *program)
{
	switch (type) {
	case GL_FLOAT:
		program->set_uniform(name, *(GLfloat *)data);
		break;
	case GL_FLOAT_VEC4:
		program->set_uniform(name, *(glm::vec4 *)data);
		break;
	case GL_SAMPLER_2D:
		program->set_uniform(name, ((glTexture *)data)->get_textureUnit());
		break;
	}
}

Uniform::~Uniform()
{
}
