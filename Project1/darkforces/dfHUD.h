#pragma once

#include "framework/fwHUDelement.h"
#include "dfBitmap.h"

class fwScene;
namespace GameEngine {
	class Level;
}

namespace DarkForces {
	class HUD {
		// hud display
		dfBitmap* m_health_bmp = nullptr;	// health and shield
		fwHUDelement* m_health = nullptr;

		dfBitmap* m_ammo_bmp = nullptr;		// ammo
		fwHUDelement* m_ammo = nullptr;

		dfBitmap* m_weapon_bmp = nullptr;	// animated weapon
		fwHUDelement* m_weapon = nullptr;
	public:
		HUD(GameEngine::Level*);
		void setWeapon(fwTexture* texture, float x, float y, float w, float h);	// change the weapon texture and X position (-1::1)
		void display(fwScene*);
		~HUD();
	};
}

extern DarkForces::HUD* g_dfHUD;