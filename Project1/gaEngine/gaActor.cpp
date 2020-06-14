#include "gaActor.h"

#include <stdio.h>
#include <iostream>

#include "../framework/fwCollision.h"
#include "../darkforces/dfLevel.h"

const float c_gravity = -0.00000981f;

gaActor::gaActor(const fwCylinder& bounding, const glm::vec3& position, float eyes, float ankle) :
	m_bounding(bounding),
	m_position(position),
	m_ankle(ankle),
	m_eyes(eyes)
{
}

void gaActor::moveForward(float speed)
{
	time_t t = 0;
	glm::vec3 move = m_direction * speed;
	collision(t, move);
}

void gaActor::collision(time_t t, glm::vec3& direction)
{
	t = 33;

	glm::vec3 intersection;
	glm::vec3 target;
	fwCollisionPoint side;

	if (m_time != 0) {
		// manage physic driven trajectory
		m_time += t;
		glm::vec3 t3x3(m_time * m_time / 2, m_time, 1);
		target = m_physic * t3x3;

		// std::cerr << "fwControlThirdPerson::update x=" << m_position.x << " y=" << m_position.y << " z=" << m_position.z << std::endl;
		// std::cerr << "fwControlThirdPerson::update TARGET x=" << target.x << " y=" << target.y << " z=" << target.z << std::endl;

		std::cerr << "fwControlThirdPerson::update delta=" << m_position.y - target.y << std::endl;

		if (m_level->checkEnvironement(m_bounding, target, intersection, side)) {
			// hit up or down => stop physic engine
			m_time = 0;
			if (side == fwCollisionPoint::BOTTOM) {
				// lock to the feet
				m_position.y = intersection.y;
			}
			else if (side == fwCollisionPoint::TOP) {
				// lock down from the head
				m_position.y = intersection.y - m_eyes;
			}
			else if (side == fwCollisionPoint::FRONT || side == fwCollisionPoint::LEFT) {
				// so, hit a wall but not a ground, stop moving and drop dow
				std::cerr << "wControlThirdPerson::update hit wall, fall down" << std::endl;
				m_time = 0;
				m_physic[0][0] = 0;			m_physic[1][0] = 0;		m_physic[2][0] = m_position.x;
				m_physic[0][1] = c_gravity;	m_physic[1][1] = 0;		m_physic[2][1] = m_position.y;
				m_physic[0][2] = 0;			m_physic[1][2] = 0;		m_physic[2][2] = m_position.z;
			}
		}
		else {
			// no collision, continue with the physic engine
			m_position = target;
		}
	}
	else {
		// move forward
		target = m_bounding.position() + direction;
		fwCylinder aabb(m_bounding, target);

		// check front collision from the ankles (to test if we can step up)
		target.y += m_ankle;

		if (m_level->checkEnvironement(m_bounding, direction, intersection, side)) {
			if (side == fwCollisionPoint::BOTTOM) {
				// test a step
				if (intersection.y - target.y > m_ankle) {
					return;	// cannot move over the step
				}

				// lock to the feet
				target.y = intersection.y;

				if (side == fwCollisionPoint::FRONT || side == fwCollisionPoint::LEFT) {
					return;	// cannot move into the wall
				}
			}
		}
		else {
			// no collision under the feet of the actor => freefall
			// engage the physic engine
			m_physic[0][0] = 0;			m_physic[1][0] = m_velocity.x;		m_physic[2][0] = m_position.x;
			m_physic[0][1] = c_gravity; m_physic[1][1] = m_velocity.y;		m_physic[2][1] = m_position.y;
			m_physic[0][2] = 0;			m_physic[1][2] = m_velocity.z;		m_physic[2][2] = m_position.z;

			m_time = t;
			std::cerr << "fwControlThirdPerson::updateCamera engage freefall x=" << m_position.x << " y=" << m_position.y << " z=" << m_position.y << std::endl;
		}
	}
}

gaActor::~gaActor()
{
}
