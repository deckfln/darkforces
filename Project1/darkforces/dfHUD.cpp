#include "dfHUD.h"

#include "../framework/fwFlatPanel.h"
#include "../gaEngine/World.h"
#include "../gaEngine/gaComponent/gaController.h"
#include "dfFileSystem.h"
#include "dfLevel.h"
#include "dfFNT.h"
#include "dfFileLFD.h"
#include "gaItem/dfItem/dfWeapon.h"

DarkForces::HUD* g_dfHUD;

static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "darkforces/shaders/hud/hud_vs.glsl"},
	{FRAGMENT_SHADER, "darkforces/shaders/hud/hud_fs.glsl"}
};

static glm::vec4 g_weaponTranform;

DarkForces::HUD::HUD(GameEngine::Level* level, fwScene* scene) :
	fwHUD("DarkForces::HUD", &g_subShaders)
{
	g_dfHUD = this;
}

/**
 * activate thre goggle green shader
 */
void DarkForces::HUD::setGoggle(bool onoff)
{
	m_material.b = onoff ? 1.0f : 0.0f;
	m_materialWeapon.b = m_material.b;
}

/**
 *
 */
void DarkForces::HUD::setScreenSize(float ratio)
{
	m_ratio = ratio / 1.6f;
}

void DarkForces::HUD::setHeadlight(bool onoff)
{
	m_materialWeapon.g = onoff ? 1.0f : 0.0f;
}

DarkForces::HUD::~HUD()
{
	delete m_materialUniform;
}
