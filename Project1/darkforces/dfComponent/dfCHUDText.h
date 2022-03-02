#pragma once

#include "../../framework/fwTexture.h"
#include "../../gaEngine/gaComponent.h"

class dfVue;
namespace DarkForces {
	namespace Component {
		class HUDtext : public gaComponent
		{
			fwTexture* m_hud = nullptr;
			int32_t m_alarmID = -1;								// registered alarm
			int32_t m_importance = -1;							// importance of currently displayed message

			void onText(gaMessage* message);					// display a text
			void onAlarm(gaMessage* message);					// remove the text

		public:
			HUDtext(void);
			HUDtext(fwTexture*);
			void dispatchMessage(gaMessage* message) override;
			// getter/setter
			inline void texture(fwTexture* texture) { m_hud = texture; };

#ifdef _DEBUG
			// debugger
			void debugGUIinline(void) override;					// Add dedicated component debug the entity
#endif
		};
	}
}
