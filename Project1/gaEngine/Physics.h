#pragma once

#include <glm/vec3.hpp>
#include "gaMessage.h"
#include "gaCollisionPoint.h"

#include <map>

class gaWorld;
class gaEntity;
class gaMessage;

static const float c_gravity = -0.00000981f;

namespace GameEngine
{
	struct Ballistic {
		time_t m_physic_time_elpased = 0;				// physic engine
		glm::mat3x3 m_physic = glm::mat3x3(0);

		Ballistic() {
		}

		Ballistic(const glm::vec3& pos, const glm::vec3& old_pos)
		{
			glm::vec3 direction = pos - old_pos;

			// engage the physic engine
			m_physic[0][0] = 0;			m_physic[1][0] = direction.x / 100.0f;		m_physic[2][0] = pos.x;
			m_physic[0][1] = c_gravity; m_physic[1][1] = direction.y / 100.0f;		m_physic[2][1] = pos.y;
			m_physic[0][2] = 0;			m_physic[1][2] = direction.z / 100.0f;		m_physic[2][2] = pos.z;

			m_physic_time_elpased = 33;
		}

		/**
		 * execute the physic engine
		 */
		void apply(time_t delta, GameEngine::Transform* transform)
		{
			// manage physic driven trajectory
			m_physic_time_elpased += delta;
			glm::vec3 t3x3(m_physic_time_elpased * m_physic_time_elpased / 2, m_physic_time_elpased, 1);
			transform->m_position = m_physic * t3x3;
		}
	};

	class Physics {
		gaWorld* m_world = nullptr;
		std::map<std::string, Ballistic> m_ballistics;	// falling objects
		std::vector<std::string> m_remove;				// list of objects to remove from the falling

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
		void update(time_t delta);
	};
}