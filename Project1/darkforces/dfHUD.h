#pragma once

#include "../framework/fwHUDelement.h"
#include "../framework/fwHUD.h"
#include "../framework/fwTexture.h"

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaImage2D.h"

#include "dfConfig.h"
#include "dfBitmap.h"
#include "dfComponent/dfCHUDText.h"
#include "dfComponent/dfCPDA.h"

class fwScene;
namespace GameEngine {
	class Level;
}

namespace DarkForces {
	class Weapon;

	class HUD : public fwHUD {
		// hud display
		dfBitmap* m_health_bmp = nullptr;	// health and shield
		GameEngine::Image2D* m_health = nullptr;

		dfBitmap* m_ammo_bmp = nullptr;		// ammo
		GameEngine::Image2D* m_ammo = nullptr;

		glm::vec4 m_materialWeapon = glm::vec4(0);

		// default data for the HUD
		glm::vec4 m_material = glm::vec4(0);
		fwUniform* m_materialUniform = nullptr;

		fwTexture m_text_bmp = fwTexture(320, 12, 4);
		GameEngine::Image2D* m_text = nullptr;		// text bar
		gaEntity m_entText = gaEntity(DarkForces::ClassID::_HUD, "hud");
		Component::HUDtext m_compText;

		// screen ration
		float m_ratio = 1.0f;

	public:
		HUD(GameEngine::Level*, fwScene* scene);
		void setGoggle(bool onoff);
		void setScreenSize(float ratio);
		void setHeadlight(bool onoff);
		~HUD();
	};
}

extern DarkForces::HUD* g_dfHUD;