#pragma once

#include "../../framework/fwTextureAtlas.h"
#include "../../framework/fwHUDelement.h"
#include "../../framework/fwHUD.h"

#include "../../gaEngine/gaComponent.h"
#include "../../gaEngine/gaUI.h"

#include "../dfFileLFD.h"

namespace DarkForces {
	namespace Component {
		class PDA : public gaComponent
		{
			DarkForces::ANIM* m_pda=nullptr;
			DarkForces::ANIM* m_guns=nullptr;
			DarkForces::ANIM* m_items=nullptr;
			DarkForces::DELT* m_pda_background=nullptr;

			GameEngine::UI* m_ui = nullptr;
			GameEngine::UI_widget* m_ui_background = nullptr;
			GameEngine::UI_widget* m_ui_weapons = nullptr;

			Framework::TextureAtlas* m_items_textures = nullptr;

			void onShowPDA(gaMessage*);							// display the PDA
			void onAddItem(gaMessage*);							// addItem on the PDA
			void onKeyDown(gaMessage*);							// deal with UI keyboard

		public:
			PDA(void);
			void dispatchMessage(gaMessage* message) override;
			GameEngine::UI* ui(void);
#ifdef _DEBUG
			// debugger
			void debugGUIinline(void) override;					// Add dedicated component debug the entity
#endif
			~PDA(void);
		};
	}
}
