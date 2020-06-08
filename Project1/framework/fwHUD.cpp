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

fwHUD::fwHUD()
{
	if (g_material == nullptr) {
		std::map<ShaderType, std::string> shaders = {
			{VERTEX_SHADER, "framework/shaders/hud/hud_vs.glsl"},
			{FRAGMENT_SHADER, "framework/shaders/hud/hud_fs.glsl"}
		};
		g_material = new fwMaterial(shaders);
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

/**
 * draw all elements
 */
void fwHUD::draw(void)
{
	for (auto element : m_elements) {
		element->draw(g_hudPanel);
	}
}

fwHUD::~fwHUD()
{
}
