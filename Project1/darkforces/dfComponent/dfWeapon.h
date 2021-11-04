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
			const DarkForces::Weapon* get(DarkForces::Weapon::Kind k);			// return data on weapons

			inline void setActorPosition(const glm::vec2& v) {
				m_ActorPosition = v;
			};																	// force the position of the weapon the player
			void dispatchMessage(gaMessage* message) override;

			// debugger
			void debugGUIinline(void) override;

		protected:
			DarkForces::Weapon::Kind m_kind;
			glm::vec2 m_ActorPosition=glm::vec2(0);				// position of the weapon on the actor

			time_t m_time=0;						// world time of the last fire (when the player keep the fire button down)

			void onFire(const glm::vec3& direction, time_t time);
		};
	}
}