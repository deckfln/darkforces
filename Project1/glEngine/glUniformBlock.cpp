#include "glUniformBlock.h"

#include "glUniform.h"

struct uniformQuery {
	GLint offset;
	GLint nameLen;
	GLint type;
	GLint elements;
};

/**
 * Parse the existing program to build the uniformBlock
 */
glUniformBlock::glUniformBlock(GLchar* name, int program, int index):
	m_name(name)
{
	GLchar uname[255];
	GLsizei length;
	GLenum type;
	GLint location;
	GLenum queryLen = GL_NAME_LENGTH;
	GLenum querySize = GL_BUFFER_DATA_SIZE;
	GLenum queryActives = GL_ACTIVE_VARIABLES;
	GLenum queryUniforms[] = { GL_OFFSET, GL_NAME_LENGTH, GL_TYPE, GL_ARRAY_SIZE };

	GLint actives[256];
	GLint l;

	uniformQuery uniform;

	glGetProgramResourceiv(program, GL_UNIFORM_BLOCK, index, 1, &querySize, 1, nullptr, &m_size);
	glGetProgramResourceiv(program, GL_UNIFORM_BLOCK, index, 1, &queryActives, 256, &length, actives);

	for (auto j = 0; j < length; j++) {
		glGetProgramResourceName(program, GL_UNIFORM, actives[j], sizeof(uname), &l, uname);
		glGetProgramResourceiv(program, GL_UNIFORM, actives[j], 4, queryUniforms, 4, nullptr, (GLint*)&uniform);

		m_uniforms[uname] = new glUniform(uname, uniform.elements, 0, uniform.type, uniform.offset);
	}
}

glUniformBlock::~glUniformBlock()
{
}
