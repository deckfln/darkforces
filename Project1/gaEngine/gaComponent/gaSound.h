#pragma once

#include <glm/vec3.hpp>
#include <map>

#include "../gaComponent.h"

#include "../../alEngine/alSource.h"
#include "../../alEngine/alSound.h"

namespace GameEngine {
	namespace Component{
		class Sound : public gaComponent{
			glm::vec3 m_position;		// origin of the sound
			static alSource m_source;
			std::map<uint32_t, alSound*> m_sounds;

		public:
			Sound(void);
			void addSound(uint32_t, alSound* sound);
			void position(const glm::vec3 &position);			// define the position of the source
			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void debugGUIinline(void) override;					// display the component in the debugger
			~Sound();
		};
	}
}