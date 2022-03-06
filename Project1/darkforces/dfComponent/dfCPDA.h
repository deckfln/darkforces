#pragma once

#include "../../framework/fwTextureAtlas.h"
#include "../../framework/fwHUDelement.h"
#include "../../gaEngine/gaComponent.h"
#include "../dfFileLFD.h"
#include "../fwHUDelement/dfHUDpda.h"

namespace DarkForces {
	namespace Component {
		class PDA : public gaComponent
		{
			static DarkForces::ANIM* m_pda;
			static DarkForces::ANIM* m_guns;
			static DarkForces::ANIM* m_items;
			static DarkForces::DELT* m_pda_background;
			static HUDelement::PDA* m_hud;

			static Framework::TextureAtlas* m_items_textures;

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
