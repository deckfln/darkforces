#include "fwMesh2D.h"

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

//---------------------------------------------

/**
 *
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

	delete m_vao;
}
