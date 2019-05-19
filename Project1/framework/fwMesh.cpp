#include "fwMesh.h"
#include "glad/glad.h"


fwMesh::fwMesh(Geometry *_geometry, Material *_material):
	geometry(_geometry),
	material(_material),
	visible(true),
	outlined(false)
{
	classID |= MESH;

	geometry->reference();
	material->reference();
}

fwMesh &fwMesh::set_visible(bool _visible)
{
	visible = _visible;
	return *this;
}

bool fwMesh::is_visible(void)
{
	return visible;
}

fwMesh &fwMesh::outline(bool _outlined)
{
	outlined = _outlined;
	return *this;
}

bool fwMesh::is_outlined(void)
{
	return outlined;
}

fwMesh &fwMesh::show_normalHelper(bool _helper)
{
	normalHelper = _helper;

	return *this;
}

bool fwMesh::is_normalHelper(void)
{
	return normalHelper;
}

void fwMesh::draw(glProgram *program)
{
	// create one VAO by shader class

	GLuint id = program->getID();
	if (vao.count(id) == 0) {
		vao[id] = new glVertexArray(geometry, program);
	}
	program->set_uniform("model", worldMatrix);

	vao[id]->draw(GL_TRIANGLES);
}

Material *fwMesh::get_material(void)
{
	return material;
}

std::string fwMesh::getMaterialHash(void)
{
	return material->hashCode();
}

fwMesh::~fwMesh()
{
	if (geometry->dereference())
		delete geometry;

	if (material->dereference())
		delete material;

	for (auto vo : vao) {
		delete vo.second;
	}
}