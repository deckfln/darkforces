#include "Mesh.h"
#include "glad/glad.h"


Mesh::Mesh(glProgram &program, Geometry &_geometry, Material &_material):
	geometry(_geometry),
	material(_material),
	vao(program, geometry, material)
{
	model = glm::mat4(1.0f);
}

void Mesh::set_uniforms(glProgram &program)
{
	program.set_uniform("model", model);
}

void Mesh::draw(glProgram &program)
{
	program.set_uniform("model", model);
	glTexture::resetTextureUnit();
	material.bindTextures(program);
	vao.draw(GL_TRIANGLES);
}

Mesh::~Mesh()
{
}
