#pragma once

#include <glm/vec3.hpp>
#include "gaMessage.h"
#include "gaCollisionPoint.h"

#include <map>

class gaEntity;
class gaMessage;

#include "../flightRecorder/Blackbox.h"
#include "../flightRecorder/Ballistic.h"

static const float c_gravity = -0.00000981f;

namespace GameEngine
{
	class World;

	struct Ballistic {
		time_t m_physic_time_elpased = 0;				// physic engine
		glm::mat3x3 m_physic = glm::mat3x3(0);
		bool m_inUse = true;

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

		Ballistic(flightRecorder::Ballistic* record) {
			m_inUse = record->inUse;
			m_physic = record->physic;
			m_physic_time_elpased = record->physic_time_elpased;
		}

		void recordState(const std::string&name, flightRecorder::Ballistic* record) {
			record->classID = flightRecorder::TYPE::PHYSICS;
			strncpy_s(record->name, name.c_str(), sizeof(record->name));
			record->inUse = m_inUse;
			record->physic = m_physic;
			record->physic_time_elpased = m_physic_time_elpased;
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

		/**
		 * Re-design the engine
		 */
		void reset(const glm::vec3& pos, const glm::vec3& old_pos)
		{
			glm::vec3 direction = pos - old_pos;

			m_physic[0][0] = 0;			m_physic[1][0] = direction.x / 100.0f;		m_physic[2][0] = pos.x;
			m_physic[0][1] = c_gravity; m_physic[1][1] = direction.y / 100.0f;		m_physic[2][1] = pos.y;
			m_physic[0][2] = 0;			m_physic[1][2] = direction.z / 100.0f;		m_physic[2][2] = pos.z;

			m_physic_time_elpased = 33;
		}


	};

	class Physics {
		std::map<std::string, Ballistic> m_ballistics;					// falling objects
		std::vector<std::string> m_remove;								// list of objects to remove from the falling
		gaEntity* m_lastEntityTested = nullptr;							// to pass information between functions

		bool warpThrough(gaEntity* entity,
			const glm::vec3& old_position,
			Transform& tranform,
		std::vector<gaCollisionPoint>& collisions);						// Test if the entity warped through a triangle
		void testEntities(gaEntity* entity, 
			const Transform& tranform, 
			std::vector<gaCollisionPoint>& collisions);					// test if the entity collide other entities
		float ifCollideWithSectorOrEntity(
			const glm::vec3& p1,
			const glm::vec3& p2,
			fwCollision::Test test, gaEntity * entity);					// test if a segment collide with a triangle of any sector
		void moveBullet(gaEntity* entity, gaMessage* message);			// simple test for bullets
		void informCollision(gaEntity* from, gaEntity* to, int flag);

		friend flightRecorder::Blackbox;

	public:
		Physics(void);
		void moveEntity(gaEntity *entity, gaMessage* message);
		void update(time_t delta);
		void recordState(const std::string& name, flightRecorder::Ballistic* object);
		void loadState(flightRecorder::Ballistic* object);
	};
}

extern GameEngine::Physics g_gaPhysics;
