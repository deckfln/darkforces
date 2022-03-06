#pragma once

#include "../../framework/fwHUDelement.h"

namespace DarkForces {
	namespace HUDelement {
		class PDA : public fwHUDelement {
		public:
			PDA(const std::string& name, Position position, fwHUDelementSizeLock lock, float width, float height, fwTexture* texture, fwFlatPanel* panel = nullptr);
			void draw(fwFlatPanel* panel) override;
		};
	}
}