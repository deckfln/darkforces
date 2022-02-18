#pragma once

#include "../framework/fwHUDelement.h"
#include "../framework/fwHUD.h"
#include "dfBitmap.h"

class fwScene;
namespace GameEngine {
	class Level;
}

namespace DarkForces {
	class HUD : public fwHUD {
		// hud display
		dfBitmap* m_health_bmp = nullptr;	// health and shield
		fwHUDelement* m_health = nullptr;

		dfBitmap* m_ammo_bmp = nullptr;		// ammo
		fwHUDelement* m_ammo = nullptr;

		dfBitmap* m_weapon_bmp = nullptr;	// animated weapon
		fwHUDelement* m_weapon = nullptr;

		glm::vec4 m_material = glm::vec4(0);
		fwUniform* m_materialUniform;

	public:
		HUD(GameEngine::Level*);
		void setWeapon(fwTexture* texture, float x, float y, float w, float h);	// change the weapon texture and X position (-1::1)
		void setGoggle(bool onoff);
		void setAmbient(float ambient);
		void setHeadlight(bool onoff);
		~HUD();
	};
}

extern DarkForces::HUD* g_dfHUD;