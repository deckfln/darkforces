#include "Mesh.h"
#include "glad/glad.h"


Mesh::Mesh(Geometry &_geometry, Material *_material):
	geometry(_geometry),
	material(_material)
{
}

void Mesh::draw(void)
{
	glProgram &program = material->get_program();

	if (vao == nullptr) {
		vao = new glVertexArray(geometry, material);
	}
	modelMatrix();
	program.set_uniform("model", model);

	glTexture::resetTextureUnit();
	material->bindTextures();
	material->set_uniforms();
	vao->draw(GL_TRIANGLES);
}

glProgram &Mesh::run(void)
{
	return material->run();
}

Mesh::~Mesh()
{
}
