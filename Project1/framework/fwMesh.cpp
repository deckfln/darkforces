#include "fwMesh.h"
#include "../glad/glad.h"

#include <imgui.h>

#include "fwMaterialDiffuse.h"
#include <string>

static int g_ids = 0;
static const char* g_className = "fwMesh";

/**
 *
 */
fwMesh::fwMesh():
	fwObject3D(),
	m_id(g_ids++)
{
	m_classID |= Framework::ClassID::MESH;
	m_className = g_className;
}

fwMesh::fwMesh(fwGeometry *_geometry, fwMaterial *_material):
	m_geometry(_geometry),
	m_material(_material),
	visible(true),
	outlined(false),
	m_id(g_ids++)
{
	m_classID |= Framework::ClassID::MESH;
	m_className = g_className;

	m_geometry->reference();
	m_material->reference();

	// for a material map, compute the tangent for each normal
	if (m_material->type(DIFFUSE_MATERIAL) && ((fwMaterialDiffuse *)m_material)->normalMap()) {
		m_geometry->computeTangent();
	}
}

fwMesh::fwMesh(fwMaterial* _material):
	m_material(_material),
	m_id(g_ids++)
{
	m_classID |= Framework::ClassID::MESH;
	m_className = g_className;
	m_material->reference();
}

/**
 * Return a shallow clone of the object
 */
fwMesh* fwMesh::clone(void)
{
	fwMesh *clone=new fwMesh(m_geometry, m_material);
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
	m_geometry = source->m_geometry;
	m_material = source->m_material;
	m_rendering = source->m_rendering;
	m_pointSize = source->m_pointSize;

	m_geometry->reference();
	m_material->reference();

	m_name = m_name + "(" + std::to_string(m_id) + ")";
}

/**
 * Init a mesh
 */
void fwMesh::set(fwGeometry* geometry, fwMaterial* material)
{
	visible = true;
	outlined = false;
	m_geometry = geometry;
	m_material = material;

	m_geometry->reference();
	m_material->reference();

	// for a material map, compute the tangent for each normal
	if (m_material->type(DIFFUSE_MATERIAL) && ((fwMaterialDiffuse*)m_material)->normalMap()) {
		m_geometry->computeTangent();
	}
}

/**
 * Build the AABB for the mesh
 */
const fwAABBox& fwMesh::modelAABB(void)
{
	return m_geometry->aabbox();
}

fwMesh &fwMesh::set_visible(bool _visible)
{
	visible = _visible;
	return *this;
}

/**
 * return the visibility of the object, if the parents are invisible, the object is considered as invisible
 */
bool fwMesh::is_visible(void)
{
	if (m_parent)
		return visible && static_cast<fwMesh*>(m_parent)->is_visible();

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
	if (m_vao.count(id) > 0 && m_geometry->resizedAttribute()) {
		delete m_vao[id];
		m_vao.erase(id);
	}

	// create one VAO by shader class
	if (m_vao.count(id) == 0) {
		m_vao[id] = new glVertexArray();
		m_geometry->enable_attributes(program);
		m_vao[id]->label(m_name.c_str());
		m_vao[id]->unbind();
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
	const glm::vec3& center = m_geometry->centerVertices();

	moveBy(center);
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
		m_geometry->draw(GL_POINTS, m_vao[id]);
		glPointSize(1.0f);
		break;
	case fwMeshRendering::FW_MESH_LINES:
		m_geometry->draw(GL_LINES, m_vao[id]);
		break;
	case fwMeshRendering::FW_MESH_LINE:
		glEnable(GL_LINE_WIDTH);
		glLineWidth(16.0f);
		m_geometry->draw(GL_LINE, m_vao[id]);
		glDisable(GL_LINE_WIDTH);
		break;
	default:
		m_geometry->draw(GL_TRIANGLES, m_vao[id]);
		break;
	}
}

fwMaterial *fwMesh::get_material(void)
{
	return m_material;
}

fwGeometry *fwMesh::get_geometry(void)
{
	return m_geometry;
}

std::string fwMesh::getMaterialHash(void)
{
	return m_material->hashCode();
}

/*
 * Refresh an attribute on the GPU
 */
void fwMesh::updateVertices(int offset, int size)
{
	m_geometry->updateVertices(offset, size);
}

void fwMesh::updateAttribute(const std::string &attribute, int offset, int size)
{
	m_geometry->updateAttribute(attribute, offset, size);
}

/**
 * SquareRoot distance from the camera to the center of the boundingSphere
 */
float fwMesh::sqDistance2boundingSphere(glm::vec3 position)
{
	return m_geometry->sqDistance2boundingSphere(position);
}

/**
 * Rendering mode of the mesh
 */
void fwMesh::rendering(fwMeshRendering render)
{
	m_rendering = render;
}

/**
 * display the mesh in a debugger
 */
void fwMesh::debugGUIChildClass(void)
{
	fwObject3D::debugGUIChildClass();
	if (ImGui::TreeNode("fwMesh")) {
		ImGui::Text("Id: %d", m_id);
		ImGui::Checkbox("Visible", &visible);
		ImGui::Checkbox("Always Draw", &m_always_draw);
		ImGui::Checkbox("Outlined", &outlined);
		ImGui::Checkbox("Transparent", &m_transparent);
		ImGui::Text("zOrder: %d", m_zorder);
		ImGui::TreePop();
	}
}

fwMesh::~fwMesh()
{
	if (m_geometry && m_geometry->dereference())
		delete m_geometry;

	if (m_material && m_material->dereference())
		delete m_material;

	for (auto vo : m_vao) {
		delete vo.second;
	}
}