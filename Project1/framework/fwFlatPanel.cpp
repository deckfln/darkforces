#include "fwFlatPanel.h"

#include "../glEngine/glProgram.h"
#include "../glEngine/glTextureArray.h"

#include "fwMaterial.h"
#include "fwGeometry.h"
#include "fwTexture.h"

static float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	-1.0f,  1.0f,
	-1.0f, -1.0f,
	 1.0f, -1.0f,

	-1.0f,  1.0f,
	 1.0f, -1.0f,
	 1.0f,  1.0f
};

static float quadUvs[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	// m_positions   // texCoords
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,

	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f
};

fwFlatPanel::fwFlatPanel(fwMaterial *material):
	m_material(material)
{
	std::string vs = m_material->load_shader(FORWARD_RENDER, VERTEX_SHADER, "");
	std::string fs = m_material->load_shader(FORWARD_RENDER, FRAGMENT_SHADER, "");
	std::string gs = m_material->load_shader(FORWARD_RENDER, GEOMETRY_SHADER, "");

	m_program = new glProgram(vs, fs, gs, "");

	m_geometry = new fwGeometry();
	m_geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
	m_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);

	m_vertexArray = new glVertexArray();
	m_geometry->enable_attributes(m_program);
	m_vertexArray->unbind();
}

/**
 * set a uniform texture
 */
void fwFlatPanel::set(const std::string& name, fwTexture* texture)
{
	m_material->set(name, texture);
}

/**
 * set a uniform texture
 */
void fwFlatPanel::set(const std::string& name, glm::vec4* v)
{
	m_material->set(name, v);
}

/**
 * Run a self-executed material (not part of the rendering process)
 */
void fwFlatPanel::draw(void)
{
	m_program->run();

	m_material->set_uniforms(m_program);
	m_geometry->draw(GL_TRIANGLES, m_vertexArray);
}

fwFlatPanel::~fwFlatPanel()
{
}
