#pragma once

#include <vector>
#include <string>
#include <map>
#include "../gaComponent.h"

class alSound;

namespace GameEngine {
	namespace Component {
		class AIPerception : public gaComponent {
			// audio perception
			bool m_audio = false;
			std::vector<uint32_t>  m_soundID;			// sounds that need to trigger a hearmessage

			// view perception
			bool m_view = false;
			float m_distance = 0.0f;						// view distance
			float m_angle = 0.0f;						// angle of the vision cone
			std::vector<std::string>  m_viewEntities;	// entities that need to trigger a view message
			std::map<uint32_t, bool> m_viewNotView;		// register the view status of each entity
			void registerEvents(void);

		public:
			AIPerception(bool view = false, bool audio = false, float distance = 0, float angle = 0);

			// getter/setter
			inline float distance(void) { return m_distance; };
			inline void distance(float d) { m_distance = d; };
			inline float angle(void) { return m_angle; };
			inline void audio(void) { m_audio = true; registerEvents(); };
			inline void view(void) { m_view = true; registerEvents(); };

			inline void hearSound(uint32_t soundID) { m_soundID.push_back(soundID); };
			inline void viewEntity(const std::string& entity) { m_viewEntities.push_back(entity); };
			inline void view(uint32_t entityID, bool b) { m_viewNotView[entityID] = b; };
			inline bool view(uint32_t entityID) {
				if (m_viewNotView.count(entityID) == 0) {
					m_viewNotView[entityID] = false;
				}
				return m_viewNotView[entityID]; 
			};

			inline const std::vector<std::string>& viewedEntities(void) { return m_viewEntities; };
			inline const std::vector<uint32_t>& heardSound(void) { return m_soundID; };

			~AIPerception();
#ifdef _DEBUG
			void debugGUIinline(void) override;					// display the component in the debugger
#endif
		};
	}
}