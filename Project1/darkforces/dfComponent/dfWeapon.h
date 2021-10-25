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
			void set(Kind k);			// set the kind of weapon

			void dispatchMessage(gaMessage* message) override;

			// debugger
			void debugGUIinline(void) override;

		protected:
			Kind m_kind;

			void onFire(const glm::vec3& direction);
		};
	}
}