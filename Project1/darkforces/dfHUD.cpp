#include "dfHUD.h"

#include "dfFileSystem.h"
#include "dfLevel.h"

DarkForces::HUD* g_dfHUD;

static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "darkforces/shaders/hud/hud_vs.glsl"},
	{FRAGMENT_SHADER, "darkforces/shaders/hud/hud_fs.glsl"}
};

DarkForces::HUD::HUD(GameEngine::Level* level) :
	fwHUD(&g_subShaders)
{
	g_dfHUD = this;

	m_materialUniform = new fwUniform("material", &m_material);
	addUniform(m_materialUniform);

	// hud display
	m_health_bmp = new dfBitmap(g_dfFiles, "STATUSLF.BM", static_cast<dfLevel*>(level)->palette());
	m_health = new fwHUDelement("statuslt", fwHUDelement::Position::BOTTOM_LEFT, fwHUDelementSizeLock::UNLOCKED, 0.2f, 0.2f, m_health_bmp->fwtexture());

	m_ammo_bmp = new dfBitmap(g_dfFiles, "STATUSRT.BM", static_cast<dfLevel*>(level)->palette());
	m_ammo = new fwHUDelement("statusrt", fwHUDelement::Position::BOTTOM_RIGHT, fwHUDelementSizeLock::UNLOCKED, 0.2f, 0.2f, m_ammo_bmp->fwtexture());

	m_weapon = new fwHUDelement("rifle", fwHUDelement::Position::BOTTOM_CENTER, fwHUDelementSizeLock::UNLOCKED, 0.4f, 0.4f, nullptr);

	add(m_health);
	add(m_ammo);
	add(m_weapon);
}

void DarkForces::HUD::setWeapon(fwTexture* texture, float x, float y, float w, float h)
{
	m_weapon->texture(texture);
	m_weapon->position(x, y);
	m_weapon->size(w, h);
}

void DarkForces::HUD::setGoggle(bool onoff)
{
	m_material.b = onoff ? 1.0f : 0.0f;
}

void DarkForces::HUD::setAmbient(float ambient)
{
	m_material.r = ambient;
}

void DarkForces::HUD::setHeadlight(bool onoff)
{
	m_material.g = onoff ? 1.0f : 0.0f;
}

DarkForces::HUD::~HUD()
{
	delete m_health;
	delete m_ammo;
	delete m_weapon;
	delete m_ammo_bmp;
	delete m_health_bmp;
	delete m_materialUniform;
}
