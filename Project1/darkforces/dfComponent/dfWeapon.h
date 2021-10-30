#pragma once

#include "../../gaEngine/gaComponent.h"

class dfBitmap;
class fwTexture;

namespace DarkForces {
	struct Weapon {
		enum class Kind {
			Concussion,
			FusionCutter,
			Missile,
			MortarGun,
			Pistol,
			PlasmaCannon,
			Repeater,
			Rifle
		};

		Kind m_kind;
		const char* m_fireSound;
		uint32_t m_damage;		// damage per bullet
		float m_recoil;			// bullet dispersion based on recoil strength
		time_t m_rate;			// how many bullets per seconds
		const char* HUDfile;	// name of the HUD image
		dfBitmap* HUDbmp;
		fwTexture* HUDtexture;
		glm::vec2 HUDposition;
	};

	namespace Component {
		class Weapon : public gaComponent {
		public:
			Weapon(void);
			Weapon(DarkForces::Weapon::Kind weapon);
			const DarkForces::Weapon* set(DarkForces::Weapon::Kind k);			// set the kind of weapon and return filename of HUD

			void dispatchMessage(gaMessage* message) override;

			// debugger
			void debugGUIinline(void) override;

		protected:
			DarkForces::Weapon::Kind m_kind;
			time_t m_time=0;			// world time of the last fire (when the player keep the fire button down)

			void onFire(const glm::vec3& direction, time_t time);
		};
	}
}