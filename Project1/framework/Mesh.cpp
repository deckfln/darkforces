#include "Mesh.h"
#include "glad/glad.h"


Mesh::Mesh(Geometry *_geometry, Material *_material):
	geometry(_geometry),
	material(_material),
	visible(true),
	outlined(false)
{
	classID |= MESH;

	geometry->reference();
	material->reference();
}

Mesh &Mesh::set_visible(bool _visible)
{
	visible = _visible;
	return *this;
}

bool Mesh::is_visible(void)
{
	return visible;
}

Mesh &Mesh::outline(bool _outlined)
{
	outlined = _outlined;
	return *this;
}

bool Mesh::is_outlined(void)
{
	return outlined;
}

void Mesh::draw(glProgram *program)
{
	// create one VAO by shader class

	GLuint id = program->getID();
	if (vao.count(id) == 0) {
		vao[id] = new glVertexArray(geometry, program);
	}
	program->set_uniform("model", worldMatrix);

	vao[id]->draw(GL_TRIANGLES);
}

Material *Mesh::get_material(void)
{
	return material;
}

std::string Mesh::getMaterialHash(void)
{
	return material->hashCode();
}

Mesh::~Mesh()
{
	if (geometry->dereference())
		delete geometry;

	if (material->dereference())
		delete material;

	for (auto vo : vao) {
		delete vo.second;
	}
}