#pragma once

#include "../../framework/fwTexture.h"
#include "../../gaEngine/gaComponent.h"
#include "../../gaEngine/gaImage2D.h"

class dfVue;
namespace DarkForces {
	namespace Component {
		class HUDtext : public gaComponent
		{
			int32_t m_alarmID = -1;								// registered alarm
			int32_t m_importance = 99999;						// importance of currently displayed message
			fwTexture m_text_bmp = fwTexture(320, 12, 4);
			GameEngine::Image2D* m_image = nullptr;

			void lazyInit(void);
			void onText(gaMessage* message);					// display a text
			void onAlarm(gaMessage* message);					// remove the text
			void onAmmo(gaMessage* message);					// display number of ammo
			void onScreenSize(gaMessage* message);
		public:
			HUDtext(void);
			HUDtext(fwTexture*);
			~HUDtext(void);
			void dispatchMessage(gaMessage* message) override;
			GameEngine::Image2D* getImage(void);
#ifdef _DEBUG
			// debugger
			void debugGUIinline(void) override;					// Add dedicated component debug the entity
#endif
		};
	}
}
