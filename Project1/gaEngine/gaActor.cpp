#include "gaActor.h"

#include <stdio.h>
#include <iostream>

#include "../framework/fwCollision.h"
#include "../darkforces/dfLevel.h"

gaActor::gaActor(fwCylinder& bounding,float eyes, float ankle) :
	m_bounding(bounding),
	m_ankle(ankle),
	m_eyes(eyes)
{
}

/**
 * change the direction vector
 */
void gaActor::rotate(const glm::vec3& direction)
{
	m_direction.x = direction.x;	// keep the Y axis from the actor
	m_direction.z = direction.z;
}

/**
 * return data from the bounding cylinder
 */
float gaActor::radius(void) 
{
	return m_bounding.radius();
}

float gaActor::height(void)
{
	return m_bounding.height();
}

/**
 * Walk folowing the front vector
 */
bool gaActor::moveTo(time_t delta, bool run)
{
	glm::vec3 direction = m_direction;
	direction.y = m_direction.y;

	delta = 33;	// TODO: remove the fixed time step

	glm::vec3 intersection;
	glm::vec3 target;
	int side = fwCollisionPoint::NONE;

	if (m_time != 0) {
		// manage physic driven trajectory
		m_time += delta;
		glm::vec3 t3x3(m_time * m_time / 2, m_time, 1);
		target = m_physic * t3x3;
		direction = target - m_bounding.position();

		// std::cerr << "fwControlThirdPerson::update x=" << m_position.x << " y=" << m_position.y << " z=" << m_position.z << std::endl;
		// std::cerr << "fwControlThirdPerson::update TARGET x=" << target.x << " y=" << target.y << " z=" << target.z << std::endl;

		std::cerr << "fwControlThirdPerson::update delta=" << m_bounding.position().y - target.y << std::endl;

		if (m_level->checkEnvironement(m_bounding, direction, intersection, side)) {
			// hit up or down => stop physic engine
			m_time = 0;
			if (side & fwCollisionPoint::BOTTOM) {
				// lock to the feet
				m_bounding.position().y = intersection.y;
			}
			else if (side & fwCollisionPoint::TOP) {
				// lock down from the head
				m_bounding.position().y = intersection.y - m_eyes;
			}
			else if ((side & fwCollisionPoint::FRONT) || (side & fwCollisionPoint::LEFT)) {
				// so, hit a wall but not a ground, stop moving and drop dow
				std::cerr << "wControlThirdPerson::update hit wall, fall down" << std::endl;
				m_time = 0;
				m_physic[0][0] = 0;			m_physic[1][0] = 0;		m_physic[2][0] = m_bounding.position().x;
				m_physic[0][1] = c_gravity;	m_physic[1][1] = 0;		m_physic[2][1] = m_bounding.position().y;
				m_physic[0][2] = 0;			m_physic[1][2] = 0;		m_physic[2][2] = m_bounding.position().z;
			}
		}
		else {
			// no move, continue with the physic engine
			m_bounding.position(target);
		}
	}
	else {
		// move forward
		target = m_bounding.position() + direction;
		fwCylinder aabb(m_bounding, target);

		// check front move from the ankles (to test if we can step up)
		target.y += m_ankle;

		if (m_level->checkEnvironement(m_bounding, direction, intersection, side)) {
			if (side == fwCollisionPoint::BOTTOM) {
				// test a step
				if (intersection.y - target.y > m_ankle) {
					return false;	// cannot move over the step
				}

				// lock to the feet
				target.y = intersection.y;

				if (side == fwCollisionPoint::FRONT || side == fwCollisionPoint::LEFT) {
					return false;	// cannot move into the wall
				}
			}
		}
		else {
			// no collision at all => nothing under the feet of the actor => freefall
			// engage the physic engine
			m_physic[0][0] = 0;			m_physic[1][0] = m_velocity.x;		m_physic[2][0] = m_bounding.position().x;
			m_physic[0][1] = c_gravity; m_physic[1][1] = m_velocity.y;		m_physic[2][1] = m_bounding.position().y;
			m_physic[0][2] = 0;			m_physic[1][2] = m_velocity.z;		m_physic[2][2] = m_bounding.position().z;

			m_time = delta;
			std::cerr << "fwControlThirdPerson::updateCamera engage freefall x=" << m_bounding.position().x << " y=" << m_bounding.position().y << " z=" << m_bounding.position().y << std::endl;
		}

		m_bounding.position(target);
	}

	return m_time != 0;	// if the physic engine is engaged
}	

/**
 * trigger a jump
 */
void gaActor::jump(void)
{
	m_velocity.y = 0.004f;

	m_physic[0][0] = 0;			m_physic[1][0] = m_velocity.x;		m_physic[2][0] = m_bounding.position().x;
	m_physic[0][1] = -0.00000981f; m_physic[1][1] = m_velocity.y;		m_physic[2][1] = m_bounding.position().y;
	m_physic[0][2] = 0;			m_physic[1][2] = m_velocity.z;		m_physic[2][2] = m_bounding.position().z;

	m_time = 33;
}

gaActor::~gaActor()
{
}
