#include "dfHUD.h"

#include "dfFileSystem.h"
#include "dfLevel.h"
#include "../framework/fwFlatPanel.h"

DarkForces::HUD* g_dfHUD;

static std::map<ShaderType, std::string> g_subShaders = {
	{VERTEX_SHADER, "darkforces/shaders/hud/hud_vs.glsl"},
	{FRAGMENT_SHADER, "darkforces/shaders/hud/hud_fs.glsl"}
};

DarkForces::HUD::HUD(GameEngine::Level* level) :
	fwHUD(&g_subShaders)
{
	g_dfHUD = this;

	m_material.r = 1.0f;
	m_materialUniform = new fwUniform("material", &m_material);
	addUniform(m_materialUniform);

	// generic hud display
	m_health_bmp = new dfBitmap(g_dfFiles, "STATUSLF.BM", static_cast<dfLevel*>(level)->palette());
	m_health = new fwHUDelement("statuslt", fwHUDelement::Position::BOTTOM_LEFT, fwHUDelementSizeLock::UNLOCKED, 0.2f, 0.2f, m_health_bmp->fwtexture());
	
	m_ammo_bmp = new dfBitmap(g_dfFiles, "STATUSRT.BM", static_cast<dfLevel*>(level)->palette());
	m_ammo = new fwHUDelement("statusrt", fwHUDelement::Position::BOTTOM_RIGHT, fwHUDelementSizeLock::UNLOCKED, 0.2f, 0.2f, m_ammo_bmp->fwtexture());

	// dedicated hud for the weapon
	m_uniWeapon = new fwUniform("material", &m_materialWeapon);

	m_panelMaterial = cloneMaterial();
	m_panelMaterial->addUniform(m_uniWeapon);
	m_panel = new fwFlatPanel(m_panelMaterial);
	m_weapon = new fwHUDelement("rifle", fwHUDelement::Position::BOTTOM_CENTER, fwHUDelementSizeLock::UNLOCKED, 0.4f, 0.4f, nullptr, m_panel);

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
	m_materialWeapon.b = m_material.b;
}

void DarkForces::HUD::setAmbient(float ambient)
{
	m_materialWeapon.r = ambient;
}

void DarkForces::HUD::setHeadlight(bool onoff)
{
	m_materialWeapon.g = onoff ? 1.0f : 0.0f;
}

DarkForces::HUD::~HUD()
{
	delete m_health;
	delete m_ammo;
	delete m_weapon;
	delete m_ammo_bmp;
	delete m_health_bmp;
	delete m_panel;
	delete m_panelMaterial;
	delete m_materialUniform;
	delete m_uniWeapon;
}
