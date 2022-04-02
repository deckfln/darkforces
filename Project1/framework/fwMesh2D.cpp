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
}

Framework::Mesh2D::Mesh2D(fwGeometry* geometry, fwMaterial* material, GLenum render):
	m_id(g_ids++),
	m_geometry(geometry),
	m_material(material),
	m_rendering(render)
{
}

void Framework::Mesh2D::set(fwGeometry* geometry, fwMaterial* material, GLenum render)
{
	m_geometry = geometry;
	m_material = material;
	m_rendering = render;
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
