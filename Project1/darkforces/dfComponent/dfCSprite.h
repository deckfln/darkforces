#pragma once

#include "../../gaEngine/gaComponent.h"

#include "../dfModel.h"

class dfFME;

namespace DarkForces {
	namespace Component {
		class Sprite : public gaComponent
		{
		protected:
			int m_dirtyAnimation = true;							// animation of the object was updated
			int m_dirtyPosition = true;								// position of the object was updated
			float m_ambient = 32.0f;								// ambient light inherited from the sector

			dfModel* m_source = nullptr;

			uint32_t m_slot;						// position of the sprite in the Sprites Managers list

			void onWorldInsert(void);
			void onWorldRemove(void);

		public:
			Sprite(dfFME* fme, float ambient);
			Sprite(dfModel* model, float ambient);

			// getter/setter
			inline const std::string& model(void) {
				return m_source->name();
			};

			// getter/setter
			inline void slot(uint32_t slot) { m_slot = slot; };

			virtual bool update(glm::vec3* position,
				glm::vec4* texture,
				glm::vec3* direction);

			void dispatchMessage(gaMessage* message) override;	// deal with messages

			// debugger
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}