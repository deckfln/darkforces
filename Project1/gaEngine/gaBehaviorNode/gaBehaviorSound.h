#pragma once

#include <vector>
#include "../../alEngine/alSound.h"
#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Sound : public BehaviorNode {
		public:
			enum class Condition {
				IN_ORDER,				// play the next sound at each run
				RANDOM					// pick randomly a sound at each run
			};
				
			Sound(const char* name);
			void execute(Action* r) override;						// let a parent take a decision with it's current running child

			void addSound(alSound* sound, uint32_t id);				// register a new sound

			//debugger
			void debugGUInode(void) override;						// display the component in the debugger
		protected:
			Condition m_condition = Condition::IN_ORDER;			// how do we play the sounds
			std::vector<alSound*> m_sounds;							// list of sounds to play
			std::vector<uint32_t> m_ids;							// ID's to bound to
			std::vector<bool> m_loaded;								// registered ?
			time_t m_maxdelay=3;									// minimum time between 2 sounds
			time_t m_delay = 0;										// minimum time between 2 sounds

			uint32_t m_currentSound = 0;
		};
	}
}