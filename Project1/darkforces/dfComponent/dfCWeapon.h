#pragma once

#include <vector>

#include "../../gaEngine/gaComponent.h"

#include "../dfWeapon.h"

class fwTexture;

namespace DarkForces {
	namespace Component {
		class Weapon : public gaComponent {
		public:
			Weapon(void);
			Weapon(DarkForces::Weapon::Kind weapon);
			DarkForces::Weapon* set(DarkForces::Weapon::Kind k);			// set the kind of weapon and return filename of HUD
			DarkForces::Weapon* get(DarkForces::Weapon::Kind k);			// return data on weapons

			inline void setActorPosition(const glm::vec2& v) {
				m_ActorPosition = v;
			};																	// force the position of the weapon the player
			void addEnergy(int32_t value);						// add energy to the weapon

			void dispatchMessage(gaMessage* message) override;

			// debugger
			void debugGUIinline(void) override;

		protected:
			DarkForces::Weapon::Kind m_kind;
			glm::vec2 m_ActorPosition=glm::vec2(0);				// position of the weapon on the actor

			time_t m_time=0;									// world time of the last fire (when the player keep the fire button down)
			uint32_t m_energy = 0;								// energy available for the weapon
			uint32_t m_maxEnergy = 200;

			void onChangeWeapon(gaMessage* message);			// change the current weapon
			void onFire(const glm::vec3& direction, time_t time);	// single shot
			void onStopFire(gaMessage* message);				// keep the finger on the trigger
			void onDying(gaMessage* message);					// drop ammo when dying
		};
	}
}