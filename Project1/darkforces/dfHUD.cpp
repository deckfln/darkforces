#include "dfHUD.h"

#include "dfFileSystem.h"
#include "dfLevel.h"

DarkForces::HUD* g_dfHUD;

DarkForces::HUD::HUD(GameEngine::Level *level)
{
	g_dfHUD = this;

	// hud display
	m_health_bmp = new dfBitmap(g_dfFiles, "STATUSLF.BM", static_cast<dfLevel*>(level)->palette());
	m_health = new fwHUDelement("statuslt", fwHUDElementPosition::BOTTOM_LEFT, fwHUDelementSizeLock::UNLOCKED, 0.1f, 0.1f, m_health_bmp->fwtexture());

	m_ammo_bmp = new dfBitmap(g_dfFiles, "STATUSRT.BM", static_cast<dfLevel*>(level)->palette());
	m_ammo = new fwHUDelement("statusrt", fwHUDElementPosition::BOTTOM_RIGHT, fwHUDelementSizeLock::UNLOCKED, 0.1f, 0.1f, m_ammo_bmp->fwtexture());

	m_weapon = new fwHUDelement("rifle", fwHUDElementPosition::BOTTOM_CENTER, fwHUDelementSizeLock::UNLOCKED, 0.2f, 0.2f, nullptr);
}

void DarkForces::HUD::setWeapon(fwTexture* texture)
{
	m_weapon->texture(texture);
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
