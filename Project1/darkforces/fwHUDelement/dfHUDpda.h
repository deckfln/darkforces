#pragma once

#include "../../framework/fwHUDelement.h"

namespace DarkForces {
	namespace HUDelement {
		class PDA : public fwHUDelement {
		public:
			PDA(const std::string& name, Position position, fwHUDelementSizeLock lock, float width, float height);
			void activateGun(uint32_t gunID);	// display the gun on the PDA
			void draw(fwFlatPanel* panel) override;
		};
	}
}