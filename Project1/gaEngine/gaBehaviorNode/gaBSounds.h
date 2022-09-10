#pragma once

#include "gaBSetVar.h"

namespace GameEngine {
	namespace Behavior {
		class Sounds : public GameEngine::Behavior::SetVar
		{
		public:
			struct Origin {
				glm::vec3 m_position;
				float m_loundness;
			};
			class Origins {
				size_t m_first=0;
				size_t m_size=0;
				size_t m_last = 0;
				Origin m_sounds[32];
			public:
				inline Origins() {};
				inline void clear(void) { 
					m_first = m_size = m_last = 0;
				};
				inline size_t size(void) {
					return m_size;
				}
				inline void push_back(const glm::vec3& position, float loundness) {
					if (m_last < 32) {
						m_sounds[m_last].m_position = position;
						m_sounds[m_last].m_loundness = loundness;
						m_size++;
						m_last++;
					}
				}
				inline const Origin& front(void) {
					m_size--;
					return m_sounds[m_first++];
				}
			};

			Sounds(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void*) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
