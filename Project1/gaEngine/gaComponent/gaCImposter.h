#pragma once

#include "../gaComponent.h"

#include <map>
#include <vector>

namespace GameEngine {
	namespace Component {
		class Imposter : public gaComponent {
			std::map<gaEntity*, bool> m_entitiesAbove;		// list of entities sitting on top of that one
			std::map<gaEntity*, bool> m_entitiesBelow;		// list of entities sitting on top of that one
		public:
			Imposter(void);
			inline void addEntityOnTop(gaEntity* entity) { m_entitiesAbove[entity] = true; };
			inline void addEntityBelow(gaEntity* entity) { m_entitiesBelow[entity] = true; };
			const std::vector<gaEntity*>& entitiesOnTop(void);

			inline void clearEntitiesOnTop(void) { m_entitiesAbove.clear(); };
			void removeEntityAbove(gaEntity* entity);
			void clearEntitiesBelow(void);
		};
	}
}