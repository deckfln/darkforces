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
			GameEngine::UI_widget* m_ui_inventory = nullptr;
			GameEngine::UI_widget* m_ui_goals = nullptr;
			GameEngine::UI_ZoomPicture* m_ui_debrief = nullptr;

			glm::vec2 m_pda_size;								// full size of the PDA image (in pixel)
			glm::vec2 m_pda_position;							// full size of the PDA image (in pixel)
			glm::vec2 m_pda_panel_size;							// size of the PDA panel (in pixel)
			glm::vec2 m_pda_panel_position;						// position of the panel (in pixel) on the PDA

			GameEngine::UI_button* buildButton(
				const std::string& name,
				uint32_t pressed,
				uint32_t message
			);													// add a button on the tab bar
			GameEngine::UI_picture* addImage(
				DarkForces::DELT* parent,
				const std::string& name,
				DarkForces::ANIM* source,
				uint32_t image,
				uint32_t textureIndex
			);													// add an image on the panel
			void addButton(
				DarkForces::DELT* parent,
				GameEngine::UI_def_button& button
			);													// add a button to the panel

			void onShowPDA(gaMessage*);							// display the PDA
			void onAddItem(gaMessage*);							// addItem on the PDA
			void onKeyDown(gaMessage*);							// deal with UI keyboard
			void onCompleteGoal(gaMessage*);					// when the player reached a goal

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
