#pragma once

#include "../../framework/fwTextureAtlas.h"
#include "../../framework/fwHUDelement.h"
#include "../../gaEngine/gaComponent.h"
#include "../../framework/fwHUD.h"

#include "../dfFileLFD.h"
#include "../fwHUDelement/dfHUDpda.h"

namespace DarkForces {
	namespace Component {
		class PDA : public gaComponent
		{
			DarkForces::ANIM* m_pda=nullptr;
			DarkForces::ANIM* m_guns=nullptr;
			DarkForces::ANIM* m_items=nullptr;
			DarkForces::DELT* m_pda_background=nullptr;

			fwHUD* m_ui = nullptr;
			HUDelement::PDA* m_ui_guns = nullptr;

			Framework::TextureAtlas* m_items_textures = nullptr;

			void onShowPDA(gaMessage*);							// display the PDA
			void onAddItem(gaMessage*);							// addItem on the PDA
			void onKeyDown(gaMessage*);							// deal with UI keyboard

		public:
			PDA(void);
			void dispatchMessage(gaMessage* message) override;
			inline fwHUD* ui(void) { return m_ui; };
#ifdef _DEBUG
			// debugger
			void debugGUIinline(void) override;					// Add dedicated component debug the entity
#endif
			~PDA(void);
		};
	}
}
