#include "fwHUD.h"

#include <map>

#include "../glEngine/glUniformBuffer.h"

#include "fwHUDelement.h"
#include "fwGeometry.h"
#include "fwUniform.h"
#include "fwMaterial.h"

static fwUniform* g_uniformTexture = nullptr;
static glm::vec4 g_screen;

static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "framework/shaders/hud/hud_vs.glsl"},
	{FRAGMENT_SHADER, "framework/shaders/hud/hud_fs.glsl"}
};

fwHUD::fwHUD(const std::string& name,  std::map<ShaderType, std::string>* shaders, bool visible):
	m_name(name),
	m_visible(visible)
{
	if (shaders == nullptr) {
		shaders = &g_subShaders;
	}
	m_material = new fwMaterial(*shaders);
	m_material->addTexture("image", (glTexture*)nullptr);
	m_material->addUniform(new fwUniform("onscreen", &g_screen));

	m_hudPanel = new fwFlatPanel(m_material);
}

/**
 * add a new element 
 */
void fwHUD::add(fwHUDelement* element)
{
	m_elements.push_back(element);
}

void fwHUD::addUniform(fwUniform* uniform)
{
	m_material->addUniform(uniform);
}

fwMaterial* fwHUD::cloneMaterial(void)
{
	return m_material->clone();
}

/**
 * draw all elements
 */
void fwHUD::draw(void)
{
	if (!m_visible) {
		return;
	}

	// always draw on top of screen
	glDisable(GL_DEPTH_TEST);								// disable depth test so screen-space quad isn't discarded due to depth test.

	for (auto element : m_elements) {
		element->draw(m_hudPanel);
	}

	glEnable(GL_DEPTH_TEST);
}

fwHUD::~fwHUD()
{
}
