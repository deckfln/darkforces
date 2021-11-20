#pragma once

#include <vector>
#include <string>

#include "../gaComponent.h"

class alSound;

namespace GameEngine {
	namespace Component {
		class AIPerception : public gaComponent {
			// audio perception
			bool m_audio = false;
			std::vector<alSound*>  m_soundID;			// sounds that need to trigger a hearmessage

			// view perception
			bool m_view = false;
			float m_distance=0.0f;						// view distance
			float m_angle = 0.0f;						// angle of the vision cone
			std::vector<std::string>  m_viewEntities;	// entities that need to trigger a view message

			void registerEvents(void);

		public:
			AIPerception(bool view=false, bool audio=false, float distance=0, float angle=0);

			// getter/setter
			inline float distance(void) { return m_distance; };
			inline void distance(float d) { m_distance = d; };
			inline float angle(void) { return m_angle; };
			inline void audio(void) { m_audio = true; registerEvents(); };
			inline void view(void) { m_view = true; registerEvents(); };

			inline void hearSound(alSound* sound) { m_soundID.push_back(sound); };
			inline void viewEntity(const std::string& entity) { m_viewEntities.push_back(entity); };

			inline const std::vector<std::string> viewedEntities(void) { return m_viewEntities; };
			inline const std::vector<alSound*> heardSound(void) { return m_soundID; };

			~AIPerception();
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}