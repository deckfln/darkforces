#pragma once

#include "../../framework/fwTexture.h"
#include "../../gaEngine/gaComponent.h"

class dfVue;
namespace DarkForces {
	namespace Component {
		class HUDtext : public gaComponent
		{
			fwTexture* m_hud = nullptr;
			void onText(gaMessage* message);					// display a text

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
