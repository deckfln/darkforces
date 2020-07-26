#include "fwMesh.h"
#include "../glad/glad.h"

#include "fwMaterialDiffuse.h"
#include <string>

static int g_ids = 0;

/**
 *
 */
fwMesh::fwMesh():
	m_id(g_ids++)
{
	classID |= MESH;
}

fwMesh::fwMesh(fwGeometry *_geometry, fwMaterial *_material):
	geometry(_geometry),
	material(_material),
	visible(true),
	outlined(false),
	m_id(g_ids++)
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
	clone->m_name = m_name + "(" + std::to_string(clone->id()) + ")";
	return clone;
}

/**
 * shallow clone by assigning data to the target
 */
void fwMesh::clone(fwMesh* source)
{
	geometry = source->geometry;
	material = source->material;
	m_rendering = source->m_rendering;
	m_pointSize = source->m_pointSize;
	m_name = m_name + "(" + std::to_string(m_id) + ")";
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
	GLuint id = program->getID();

	// if one of the attribute was resized, need to rebuild a vao with the new attribute
	if (vao.count(id) > 0 && geometry->resizedAttribute()) {
		delete vao[id];
		vao.erase(id);
	}

	// create one VAO by shader class
	if (vao.count(id) == 0) {
		vao[id] = new glVertexArray();
		geometry->enable_attributes(program);
		vao[id]->label(m_name.c_str());
		vao[id]->unbind();
	}

	// update the mesh model
	program->set_uniform("model", m_worldMatrix);
	this->set_uniforms(program);

	return id;
}

/**
 * Center the mesh on the center of the geometry
 */
void fwMesh::centerOnGeometry(void)
{
	const glm::vec3& center = geometry->centerVertices();

	m_Position += center;
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
	case fwMeshRendering::FW_MESH_LINES:
		geometry->draw(GL_LINES, vao[id]);
		break;
	case fwMeshRendering::FW_MESH_LINE:
		glEnable(GL_LINE_WIDTH);
		glLineWidth(16.0f);
		geometry->draw(GL_LINE, vao[id]);
		glDisable(GL_LINE_WIDTH);
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