#include "fwHUD.h"

#include <map>

#include "../glEngine/glUniformBuffer.h"

#include "fwHUDelement.h"
#include "fwGeometry.h"
#include "fwUniform.h"
#include "fwMaterial.h"
#include "fwFlatPanel.h"

static fwFlatPanel* g_hudPanel=nullptr;
static fwMaterial* g_material = nullptr;
static fwUniform* g_uniformTexture = nullptr;
static glm::vec4 g_screen;

static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "framework/shaders/hud/hud_vs.glsl"},
	{FRAGMENT_SHADER, "framework/shaders/hud/hud_fs.glsl"}
};

fwHUD::fwHUD(std::map<ShaderType, std::string>* shaders)
{
	if (g_material == nullptr) {
		if (shaders == nullptr) {
			shaders = &g_subShaders;
		}
		g_material = new fwMaterial(*shaders);
		g_material->addTexture("image", (glTexture*)nullptr);
		g_material->addUniform(new fwUniform("onscreen", &g_screen));

		g_hudPanel = new fwFlatPanel(g_material);
	}
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
	g_material->addUniform(uniform);
}

/**
 * draw all elements
 */
void fwHUD::draw(void)
{
	// always draw on top of screen
	glDisable(GL_DEPTH_TEST);								// disable depth test so screen-space quad isn't discarded due to depth test.

	for (auto element : m_elements) {
		element->draw(g_hudPanel);
	}

	glEnable(GL_DEPTH_TEST);
}

fwHUD::~fwHUD()
{
}
