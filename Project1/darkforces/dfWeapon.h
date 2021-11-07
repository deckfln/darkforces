#pragma once

#include <vector>

#include "../framework/fwTexture.h"

#include "dfPalette.h"
#include "dfBitmap.h"
#include "dfFileSystem.h"

namespace DarkForces {
	class Weapon {
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
		const char* debug;
		const char* m_fireSound;
		uint32_t m_damage;		// damage per bullet
		float m_recoil;			// bullet dispersion based on recoil strength
		time_t m_rate;			// how many bullets per seconds
		std::vector<const char*> HUDfiles;	// name of the HUD images
		std::vector<dfBitmap*> HUDbmps;
		glm::vec2 HUDposition;

		fwTexture* getStillTexture(dfPalette* palette) {
			if (HUDbmps[0] == nullptr) {
				HUDbmps[0] = new dfBitmap(g_dfFiles, HUDfiles[0], palette);
			}
			return HUDbmps[0]->fwtexture();
		}
		fwTexture* getFireTexture(dfPalette* palette) {
			if (HUDbmps[1] == nullptr) {
				HUDbmps[1] = new dfBitmap(g_dfFiles, HUDfiles[1], palette);
			}
			return HUDbmps[1]->fwtexture();
		}
	};
}