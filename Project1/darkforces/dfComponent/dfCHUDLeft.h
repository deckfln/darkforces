#pragma once

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaImage2D.h"
#include "../dfBitmap.h"

namespace DarkForces {
	namespace Component {
		class HUDLeft : public gaComponent {
			GameEngine::Image2D* m_image = nullptr;
			dfBitmap* m_statuslf = nullptr;
			void lazyInit(void);
			void onScreenSize(gaMessage*);
			void onLife(gaMessage*);
			void onShield(gaMessage*);
		public:
			HUDLeft(void);
			void dispatchMessage(gaMessage* message) override;
			GameEngine::Image2D* getImage(void);					// build and return an Image2D for the HUD
			~HUDLeft(void);
		};
	}
}