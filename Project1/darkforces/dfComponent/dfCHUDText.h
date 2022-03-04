#pragma once

#include "../../framework/fwTexture.h"
#include "../../gaEngine/gaComponent.h"

class dfVue;
namespace DarkForces {
	namespace Component {
		class HUDtext : public gaComponent
		{
			fwTexture* m_hud = nullptr;
			fwTexture* m_ammo = nullptr;
			fwTexture* m_left = nullptr;
			int32_t m_alarmID = -1;								// registered alarm
			int32_t m_importance = -1;							// importance of currently displayed message

			void onText(gaMessage* message);					// display a text
			void onAlarm(gaMessage* message);					// remove the text
			void onAmmo(gaMessage* message);					// display number of ammo
			void onShield(gaMessage* message);					// display number of shield

		public:
			HUDtext(void);
			HUDtext(fwTexture*);
			void dispatchMessage(gaMessage* message) override;
			// getter/setter
			inline void texture(fwTexture* texture) { m_hud = texture; };
			inline void ammo(fwTexture* texture) { m_ammo = texture; };
			inline void shield(fwTexture* texture) { m_left = texture; };

#ifdef _DEBUG
			// debugger
			void debugGUIinline(void) override;					// Add dedicated component debug the entity
#endif
		};
	}
}
