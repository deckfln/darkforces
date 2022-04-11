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
 *
 */
void GameEngine::Image2D::setGeometry(void)
{
	if (g_geometry == nullptr) {
		g_geometry = new fwGeometry();
		g_geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
		g_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);
	}
	m_geometry = g_geometry;
}

void GameEngine::Image2D::setMaterial(void)
{
	if (g_material == nullptr) {
		g_material = new fwMaterial(g_subShaders);
		g_material->addTexture("image", (glTexture*)nullptr);
		g_material->addUniform(new fwUniform("transformation", &g_screen));
	}
	m_material = g_material;
}

void GameEngine::Image2D::setUniforms(void)
{
	// define per mesh value of the materials uniforms
	add_uniform("image", m_texture);
}


GameEngine::Image2D::Image2D(const std::string& name, const glm::vec2& _scale, const glm::vec2& _translate, fwTexture* texture, fwMaterial* material):
	Framework::Mesh2D(name),
	m_texture(texture)
{
	setGeometry();

	scale(_scale);
	translate(_translate);

	if (material != nullptr) {
		m_material = material;
	}
	else {
		setMaterial();
	}

	setUniforms();
}

/**
 * create with a shader
 */
GameEngine::Image2D::Image2D(const std::string& name, const glm::vec2& _scale, const glm::vec2& _translate, fwTexture* texture, const std::map<ShaderType, std::string>& shaders) :
	Framework::Mesh2D(name),
	m_texture(texture)
{
	setGeometry();

	scale(_scale);
	translate(_translate);

	m_material = new fwMaterial(shaders);
	m_material->addTexture("image", (glTexture*)nullptr);
	g_material->addUniform(new fwUniform("transformation", &g_screen));

	setUniforms();
}

/**
 *
 */
void GameEngine::Image2D::setTexture(fwTexture* texture)
{
	m_texture = texture;
	add_uniform("image", texture);
}