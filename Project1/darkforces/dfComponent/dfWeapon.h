#pragma once

#include "../../gaEngine/gaComponent.h"

namespace DarkForces {
	namespace Component {
		class Weapon : public gaComponent {
		public:
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

			Weapon(void);
			Weapon(Kind weapon);
			const char* set(Kind k);			// set the kind of weapon and return filename of HUD

			void dispatchMessage(gaMessage* message) override;

			// debugger
			void debugGUIinline(void) override;

		protected:
			Kind m_kind;	
			time_t m_time=0;			// world time of the last fire (when the player keep the fire button down)

			void onFire(const glm::vec3& direction, time_t time);
		};
	}
}