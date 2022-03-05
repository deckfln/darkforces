#pragma once

#include "../../framework/fwTexture.h"
#include "../../framework/fwHUDelement.h"
#include "../../gaEngine/gaComponent.h"
#include "../dfFileLFD.h"

namespace DarkForces {
	namespace Component {
		class PDA : public gaComponent
		{
			static DarkForces::ANIM* m_pda;
			static DarkForces::ANIM* m_guns;
			static DarkForces::ANIM* m_items;
			static DarkForces::DELT* m_pda_background;
			static fwHUDelement* m_hud;

			void onWorldInsert(gaMessage*);						// initialize the PDA
			void onShowPDA(gaMessage*);							// display the PDA

		public:
			PDA(void);
			void dispatchMessage(gaMessage* message) override;
			fwHUDelement* hud(void);

#ifdef _DEBUG
			// debugger
			void debugGUIinline(void) override;					// Add dedicated component debug the entity
#endif
			~PDA(void);
		};
	}
}
