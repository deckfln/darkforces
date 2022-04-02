#include "gaImage2D.h"

#include "../framework/fwGeometry.h"
#include "../framework/fwMaterial.h"
#include "../framework/fwUniform.h"

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
static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "framework/shaders/hud/hud_vs.glsl"},
	{FRAGMENT_SHADER, "framework/shaders/hud/hud_fs.glsl"}
};

static fwGeometry* g_geometry;
static fwMaterial* g_material;
static fwUniform* g_uniformTexture = nullptr;
static glm::vec4 g_screen;

/**
 * create
 */
GameEngine::Image2D::Image2D(const std::string& name, float x, float y, float width, float height, fwTexture* texture, fwMaterial* material):
	Framework::Mesh2D(name),
	m_onscreen(x, y, width, height),
	m_texture(texture)
{
	if (g_geometry == nullptr) {
		g_geometry = new fwGeometry();
		g_geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
		g_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);
	}
	m_geometry = g_geometry;

	if (g_material == nullptr) {
		g_material = new fwMaterial(g_subShaders);
		g_material->addTexture("image", (glTexture*)nullptr);
		g_material->addUniform(new fwUniform("onscreen", &g_screen));

	}
	if (m_material != nullptr) {
		m_material = material;
	}
	else {
		m_material = g_material;
	}

	// define per mesh value of the materials uniforms
	add_uniform("image", m_texture);
	add_uniform("onscreen", m_onscreen);
}

/**
 * create with a shader
 */
GameEngine::Image2D::Image2D(const std::string& name, float x, float y, float width, float height, fwTexture* texture, const std::map<ShaderType, std::string>& shaders) :
	Framework::Mesh2D(name),
	m_onscreen(x, y, width, height),
	m_texture(texture)
{
	if (g_geometry == nullptr) {
		g_geometry = new fwGeometry();
		g_geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
		g_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);
	}
	m_geometry = g_geometry;

	m_material = new fwMaterial(shaders);
	m_material->addTexture("image", (glTexture*)nullptr);
	m_material->addUniform(new fwUniform("onscreen", &m_onscreen));

	// define per mesh value of the materials uniforms
	add_uniform("image", m_texture);
	add_uniform("onscreen", m_onscreen);
}
