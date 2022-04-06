#include "fwMesh2D.h"

#include "fwUniform.h"

//---------------------------------------------

static uint32_t g_ids = 0;

Framework::Mesh2D::Mesh2D():
	m_id(g_ids++)
{
}

Framework::Mesh2D::Mesh2D(const std::string& name):
	m_id(g_ids++),
	m_name(name)
{
	add_uniform("transformation", m_transformation);
}

Framework::Mesh2D::Mesh2D(fwGeometry* geometry, fwMaterial* material, GLenum render):
	m_id(g_ids++),
	m_geometry(geometry),
	m_material(material),
	m_rendering(render)
{
	add_uniform("transformation", m_transformation);
}

void Framework::Mesh2D::set(fwGeometry* geometry, fwMaterial* material, GLenum render)
{
	m_geometry = geometry;
	m_material = material;
	m_rendering = render;

	add_uniform("transformation", m_transformation);
}

/**
 * Set local uniform values for the material
 */
void Framework::Mesh2D::add_uniform(const std::string& name, const glm::vec4& vec4)
{
	m_materialUniforms[name] = {
		GL_FLOAT_VEC4,
		&vec4
	};
}

void Framework::Mesh2D::add_uniform(const std::string& name, const fwTexture* texture)
{
	m_materialUniforms[name] = {
		GL_SAMPLER_2D,
		texture
	};
}

//---------------------------------------------

/**
 *  set uniforms per meshe
 */
void Framework::Mesh2D::set_uniforms(glProgram* program)
{
	// set the local values of material uniforms
	if (m_materialUniforms.size() > 0) {
		for (auto& uniform : m_materialUniforms) {
			switch (uniform.second.m_type) {
			case GL_FLOAT_VEC4:
				m_material->set(uniform.first, (glm::vec4*)uniform.second.m_value);
				break;
			case GL_SAMPLER_2D:
				m_material->set(uniform.first, (fwTexture*)uniform.second.m_value);
				break;
			}
		}
		m_material->set_uniforms(program);
	}

	// activate the uniq uniforms of the mesh
	for (auto uniform : m_uniforms) {
		uniform->set_uniform(program);
	}

}

/**
 * update world matrix
 */
void Framework::Mesh2D::updateWorld(Framework::Mesh2D* parent, bool force)
{
	if (m_dirty || force) {
		m_dirty = false;
		if (parent != nullptr) {
			m_gscale = parent->m_gscale * m_scale;
			m_gtranslation = parent->m_gtranslation + m_translation * parent->m_gscale;
		}
		else {
			m_gscale = m_scale;
			m_gtranslation = m_translation;
		}

		m_transformation.x = m_gscale.x;
		m_transformation.y = m_gscale.y;
		m_transformation.w = m_gtranslation.x;
		m_transformation.z = m_gtranslation.y;

		for (auto child : m_children) {
			child->updateWorld(this, true);
		}
	}
}

/**
 * draw the mesh
 */
void Framework::Mesh2D::draw(glVertexArray* vao)
{
	m_geometry->draw(m_rendering, vao);
}

//---------------------------------------------

Framework::Mesh2D::~Mesh2D()
{
	if (m_geometry && m_geometry->dereference())
		delete m_geometry;

	if (m_material && m_material->dereference())
		delete m_material;
}
