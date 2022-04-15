#pragma once

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaImage2D.h"
#include "../dfBitmap.h"

namespace DarkForces {
	namespace Component {
		class HUDRight : public gaComponent {
			GameEngine::Image2D* m_image = nullptr;
			dfBitmap* m_statuslf = nullptr;
			float m_ratio = 1.6f;

			void lazyInit(void);
			void onScreenSize(gaMessage*);
			void onAmmo(gaMessage* message);
		public:
			HUDRight(void);
			void dispatchMessage(gaMessage* message) override;
			GameEngine::Image2D* getImage(void);					// build and return an Image2D for the HUD
			~HUDRight(void);
		};
	}
}