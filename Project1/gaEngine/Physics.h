#pragma once

#include <glm/vec3.hpp>
#include "gaMessage.h"
#include "gaCollisionPoint.h"

class gaWorld;
class gaEntity;
class gaMessage;

namespace GameEngine
{
	class Physics {
		gaWorld* m_world = nullptr;
		bool warpThrough(gaEntity* entity,
			const glm::vec3& old_position,
			Transform& tranform);						// Test if the entity warped through a triangle
		void testEntities(gaEntity* entity, 
			const Transform& tranform, 
			std::vector<gaCollisionPoint>& collisions);	// test if the entity collide other entities
		void testSectors(gaEntity* entity,
			const Transform& tranform,
			std::vector<gaCollisionPoint>& collisions);	// test if the entity collide sectors

	public:
		Physics(gaWorld *world);
		void moveEntity(gaEntity *entity, gaMessage* message);
	};
}