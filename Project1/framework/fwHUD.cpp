#include "fwHUD.h"

#include <map>

#include "../glEngine/glProgram.h"
#include "../glEngine/glUniformBuffer.h"

#include "fwHUDelement.h"
#include "fwGeometry.h"
#include "fwUniform.h"
#include "fwMaterial.h"

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

static glProgram* g_program=nullptr;
static fwMaterial* g_material = nullptr;
static fwUniform* g_uniformTexture = nullptr;
static fwGeometry* g_geometry = nullptr;
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

		g_geometry = new fwGeometry();
		g_geometry->addVertices("aPos", quadVertices, 2, sizeof(quadVertices), sizeof(float), false);
		g_geometry->addAttribute("aTex", GL_ARRAY_BUFFER, quadUvs, 2, sizeof(quadUvs), sizeof(float), false);
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
		element->draw(g_material, g_geometry);
	}
}

fwHUD::~fwHUD()
{
}
