#include "dfHUD.h"

#include "dfFileSystem.h"
#include "dfLevel.h"

DarkForces::HUD* g_dfHUD;

DarkForces::HUD::HUD(GameEngine::Level *level)
{
	g_dfHUD = this;

	// hud display
	m_health_bmp = new dfBitmap(g_dfFiles, "STATUSLF.BM", static_cast<dfLevel*>(level)->palette());
	m_health = new fwHUDelement("statuslt", fwHUDelement::Position::BOTTOM_LEFT, fwHUDelementSizeLock::UNLOCKED, 0.2f, 0.2f, m_health_bmp->fwtexture());

	m_ammo_bmp = new dfBitmap(g_dfFiles, "STATUSRT.BM", static_cast<dfLevel*>(level)->palette());
	m_ammo = new fwHUDelement("statusrt", fwHUDelement::Position::BOTTOM_RIGHT, fwHUDelementSizeLock::UNLOCKED, 0.2f, 0.2f, m_ammo_bmp->fwtexture());

	m_weapon = new fwHUDelement("rifle", fwHUDelement::Position::BOTTOM_CENTER, fwHUDelementSizeLock::UNLOCKED, 0.4f, 0.4f, nullptr);
}

void DarkForces::HUD::setWeapon(fwTexture* texture, float x, float y, float w, float h)
{
	m_weapon->texture(texture);
	m_weapon->position(x, y);
	m_weapon->size(w, h);
}

void DarkForces::HUD::display(fwScene* scene)
{
	scene->hud(m_health);
	scene->hud(m_ammo);
	scene->hud(m_weapon);
}

DarkForces::HUD::~HUD()
{
}
