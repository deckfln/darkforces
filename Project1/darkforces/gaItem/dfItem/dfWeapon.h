#pragma once

#include <vector>
#include <map>
#include <glm/vec2.hpp>

#include "../../../framework/fwTexture.h"

#include "../dfItem.h"
#include "../../dfObject.h"
#include "../../dfPalette.h"
#include "../../dfBitmap.h"
#include "../../dfFileSystem.h"

namespace DarkForces {
	class Weapon: public DarkForces::Item {
		uint32_t m_energy = 0;					// current bullets
		uint32_t m_maxEnergy = 0;				// maximum bullets in the clip

		void onDropItem(gaMessage* message);

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
		uint32_t m_damage;						// damage per bullet
		float m_recoil;							// bullet dispersion based on recoil strength
		time_t m_rate;							// how many bullets per seconds
		std::vector<const char*> m_HUDfiles;	// name of the HUD images
		std::vector<dfBitmap*> m_HUDbmps;
		glm::vec2 m_HUDposition;

		DarkForces::Weapon(void):
			DarkForces::Item("none", dfLogic::NONE)
		{
		};

		DarkForces::Weapon(DarkForces::Weapon* source):
			DarkForces::Item(source->m_debug, dfLogic::WEAPON),
			m_kind(source->m_kind),
			m_debug(source->m_debug),
			m_fireSound(source->m_fireSound),
			m_damage(source->m_damage),
			m_recoil(source->m_recoil),
			m_rate(source->m_rate),
			m_HUDfiles(source->m_HUDfiles),
			m_HUDbmps(source->m_HUDbmps),
			m_HUDposition(source->m_HUDposition),
			m_energy(source->m_energy)
		{
		};

		DarkForces::Weapon(
			const Kind kind,
			const char* debug,
			const char* fireSound,
			const uint32_t damage,
			const float recoil,
			const time_t rate,
			const uint32_t energy,		// default energy
			const uint32_t maxEnergy,	// maximum energy
			const std::vector<const char*>& HUDfiles,
			const std::vector<dfBitmap*>& HUDbmps,
			const glm::vec2& HUDposition
		) :
			DarkForces::Item(debug, dfLogic::WEAPON),
			m_kind(kind),
			m_debug(debug),
			m_fireSound(fireSound),
			m_damage(damage),
			m_recoil(recoil),
			m_rate(rate),
			m_energy(energy),
			m_maxEnergy(maxEnergy),
			m_HUDfiles(HUDfiles),
			m_HUDbmps(HUDbmps),
			m_HUDposition(HUDposition)
		{
		};

		void clone(DarkForces::Weapon* source);			// clone the weapon
		void loadClip(void);							// load the clip with energy

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

		void dispatchMessage(gaMessage* message) override;

		inline Kind kind(void) { return m_kind; };

		uint32_t energy(void);
		uint32_t decreaseEnergy(void);					// remove a bullet from the weapon
		uint32_t addEnergy(uint32_t v);					// remove a bullet from the weapon
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

