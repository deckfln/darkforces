#include "Mesh.h"
#include "glad/glad.h"


Mesh::Mesh(Geometry &_geometry, Material *_material):
	geometry(_geometry),
	material(_material)
{
}

void Mesh::draw(glProgram *program)
{
	if (vao == nullptr) {
		vao = new glVertexArray(geometry, program);
	}
	modelMatrix();
	program->set_uniform("model", model);

	glTexture::resetTextureUnit();
	material->bindTextures();
	material->set_uniforms(program);
	vao->draw(GL_TRIANGLES);
}

Material *Mesh::get_material(void)
{
	return material;
}

std::string Mesh::getMaterialHash(void)
{
	return material->hash();
}

Mesh::~Mesh()
{
	if (vao != nullptr) 
		delete vao;
}
