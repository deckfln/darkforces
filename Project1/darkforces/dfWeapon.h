#pragma once

#include <vector>
#include <map>
#include <glm/vec2.hpp>

#include "../framework/fwTexture.h"

#include "../gaEngine/gaItem.h"

#include "dfPalette.h"
#include "dfBitmap.h"
#include "dfFileSystem.h"

namespace DarkForces {
	class Weapon: public GameEngine::Item {
	public:
		enum class Kind {
			Concussion,
			FusionCutter,
			Missile,
			MortarGun,
			Pistol,
			PlasmaCannon,
			Repeater,
			Rifle,
			None,
			Punch
		};

		Kind m_kind;
		const char* m_debug;
		const char* m_fireSound;
		uint32_t m_damage;		// damage per bullet
		float m_recoil;			// bullet dispersion based on recoil strength
		time_t m_rate;			// how many bullets per seconds
		std::vector<const char*> m_HUDfiles;	// name of the HUD images
		std::vector<dfBitmap*> m_HUDbmps;
		glm::vec2 m_HUDposition;

		DarkForces::Weapon(
			const Kind kind,
			const char* debug,
			const char* fireSound,
			const uint32_t damage,
			const float recoil,
			const time_t rate,
			const std::vector<const char*>& HUDfiles,
			const std::vector<dfBitmap*>& HUDbmps,
			const glm::vec2& HUDposition
		) :
			GameEngine::Item(debug),
			m_kind(kind),
			m_debug(debug),
			m_fireSound(fireSound),
			m_damage(damage),
			m_recoil(recoil),
			m_rate(rate),
			m_HUDfiles(HUDfiles),
			m_HUDbmps(HUDbmps),
			m_HUDposition(HUDposition)
		{

		};

		fwTexture* getStillTexture(dfPalette* palette) {
			if (m_HUDbmps[0] == nullptr) {
				m_HUDbmps[0] = new dfBitmap(g_dfFiles, m_HUDfiles[0], palette);
			}
			return m_HUDbmps[0]->fwtexture();
		}
		fwTexture* getFireTexture(dfPalette* palette) {
			if (m_HUDbmps[1] == nullptr) {
				m_HUDbmps[1] = new dfBitmap(g_dfFiles, m_HUDfiles[1], palette);
			}
			return m_HUDbmps[1]->fwtexture();
		}
	};
}

extern DarkForces::Weapon g_concussion;
extern DarkForces::Weapon g_FusionCutter;
extern DarkForces::Weapon g_Missile;
extern DarkForces::Weapon g_MortarGun;
extern DarkForces::Weapon g_Pistol;
extern DarkForces::Weapon g_PlasmaCannon;
extern DarkForces::Weapon g_Repeater;
extern DarkForces::Weapon g_Rifle;

