#include "fwMesh.h"
#include "../glad/glad.h"

#include "fwMaterialDiffuse.h"

fwMesh::fwMesh()
{
	classID |= MESH;
}

fwMesh::fwMesh(fwGeometry *_geometry, fwMaterial *_material):
	geometry(_geometry),
	material(_material),
	visible(true),
	outlined(false)
{
	classID |= MESH;

	geometry->reference();
	material->reference();

	// for a material map, compute the tangent for each normal
	if (material->type(DIFFUSE_MATERIAL) && ((fwMaterialDiffuse *)material)->normalMap()) {
		geometry->computeTangent();
	}
}

/**
 * Return a shallow clone of the object
 */
fwMesh* fwMesh::clone(void)
{
	fwMesh *clone=new fwMesh(geometry, material);
	clone->m_rendering = m_rendering;
	clone->m_pointSize = m_pointSize;
	return clone;
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

fwMesh &fwMesh::draw_wireframe(bool _wireframe)
{
	wireFrame = _wireframe;
	return *this;
}

fwMesh& fwMesh::addUniform(fwUniform* uniform)
{
	m_uniforms.push_front(uniform);
	return *this;
}

void fwMesh::set_uniforms(glProgram* program)
{
	for (auto uniform : m_uniforms) {
		uniform->set_uniform(program);
	}
}

GLuint fwMesh::buildVAO(glProgram* program)
{
	// create one VAO by shader class
	GLuint id = program->getID();
	if (vao.count(id) == 0) {
		vao[id] = new glVertexArray();
		geometry->enable_attributes(program);
		vao[id]->unbind();
	}

	// update the mesh model
	program->set_uniform("model", m_worldMatrix);
	this->set_uniforms(program);

	return id;
}

void fwMesh::draw(glProgram *program)
{
	static GLint renders[] = {
		GL_TRIANGLES,
		GL_POINT,
		GL_LINES
	};

	GLuint id = buildVAO(program);

	switch (m_rendering) {
	case fwMeshRendering::FW_MESH_POINT:
		glPointSize(m_pointSize);
		geometry->draw(GL_POINTS, vao[id]);
		glPointSize(1.0f);
		break;
	case fwMeshRendering::FW_MESH_LINE:
		geometry->draw(GL_LINE, vao[id]);
		break;
	default:
		geometry->draw(GL_TRIANGLES, vao[id]);
		break;
	}
}

fwMaterial *fwMesh::get_material(void)
{
	return material;
}

fwGeometry *fwMesh::get_geometry(void)
{
	return geometry;
}

std::string fwMesh::getMaterialHash(void)
{
	return material->hashCode();
}

/*
 * Refresh an attribute on the GPU
 */
void fwMesh::updateVertices(int offset, int size)
{
	geometry->updateVertices(offset, size);
}

void fwMesh::updateAttribute(const std::string &attribute, int offset, int size)
{
	geometry->updateAttribute(attribute, offset, size);
}

/**
 * SquareRoot distance from the camera to the center of the boundingSphere
 */
float fwMesh::sqDistance2boundingSphere(glm::vec3 position)
{
	return geometry->sqDistance2boundingSphere(position);
}

/**
 * Rendering mode of the mesh
 */
void fwMesh::rendering(fwMeshRendering render)
{
	m_rendering = render;
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