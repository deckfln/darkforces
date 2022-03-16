#pragma once

#include "../../framework/fwTextureAtlas.h"
#include "../../framework/fwHUDelement.h"
#include "../../framework/fwHUD.h"

#include "../../gaEngine/gaComponent.h"
#include "../../gaEngine/gaUI.h"

#include "../dfFileLFD.h"

namespace DarkForces {
	namespace Component {
		class PDA : public GameEngine::UI
		{
			DarkForces::ANIM* m_pda=nullptr;
			DarkForces::ANIM* m_guns=nullptr;
			DarkForces::ANIM* m_items=nullptr;
			DarkForces::DELT* m_pda_background=nullptr;

			GameEngine::UI_widget *m_ui_weapons = nullptr;
			GameEngine::UI_ZoomPicture* m_ui_debrief = nullptr;

			void onShowPDA(gaMessage*);							// display the PDA
			void onAddItem(gaMessage*);							// addItem on the PDA
			void onKeyDown(gaMessage*);							// deal with UI keyboard

		public:
			PDA(const std::string& name);
			void dispatchMessage(gaMessage* message) override;
#ifdef _DEBUG
			// debugger
			void debugGUIinline(void) override;					// Add dedicated component debug the entity
#endif
			~PDA(void);
		};
	}
}
