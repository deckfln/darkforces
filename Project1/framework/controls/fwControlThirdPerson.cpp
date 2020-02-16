#include "fwControlThirdPerson.h"

#include <GLFW/glfw3.h>

fwControlThirdPerson::fwControlThirdPerson(fwCamera* _camera, glm::vec3 position, glm::vec3 direction, float radius) :
	fwControl(_camera),
	m_position(position),
	m_direction(direction),
	m_radius(radius)
{
	updateCamera(0);
}

/**
 * start tracking the mouse to update the camera angle 
 */
void fwControlThirdPerson::_mouseButton(int action)
{
	switch (m_button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			m_phi_start = m_phi;
			m_theta_start = m_theta;
		}
		else {
			updateDirection();
		}
		break;
	}
}

/**
 * update the camera angle based on mouse movement
 */
void fwControlThirdPerson::_mouseMove(float xdir, float ydir)
{
	// std::cout << xdir << "  " << ydir << std::endl;

	switch (m_button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		m_phi = m_phi_start + xdir * m_radSpeed;
		m_theta = m_theta_start + ydir * m_radSpeed;
		break;
	}
}


/**
 * deal with movement keys
 */
void fwControlThirdPerson::checkKeys(time_t delta)
{
	float running = m_shift ? 2.0f : 1.0f;

	if (!m_freefall) {
		if (m_currentKeys[GLFW_KEY_UP]) {
			m_velocity = m_direction * m_speed * running;
		}
		if (m_currentKeys[GLFW_KEY_DOWN]) {
			m_velocity = -m_direction * m_speed * running;
		}

		if (!m_currentKeys[GLFW_KEY_DOWN] && !m_currentKeys[GLFW_KEY_UP]) {
			m_velocity.x = m_velocity.z = 0.0f;
		}
		if (m_currentKeys[GLFW_KEY_LEFT]) {
			m_phi -= 0.003 * delta;
			updateDirection();
		}
		if (m_currentKeys[GLFW_KEY_RIGHT]) {
			m_phi += 0.003 * delta;
			updateDirection();
		}
		if (m_currentKeys[GLFW_KEY_X]) {
			m_debug = m_position;

			m_velocity.y = 0.004f;

			m_physic[0][0] = 0;			m_physic[1][0] = m_velocity.x;		m_physic[2][0] = m_position.x;
			m_physic[0][1] = -0.00000981f; m_physic[1][1] = m_velocity.y;		m_physic[2][1] = m_position.y;
			m_physic[0][2] = 0;			m_physic[1][2] = m_velocity.z;		m_physic[2][2] = m_position.z;

			m_freefall = true;
			m_time = 0;
		}
	}
}

/**
 * Move the player in the given sirection if there is no collision
 */
bool fwControlThirdPerson::checkCollision(glm::vec3& target)
{
	glm::vec3 intersection;

	return m_collision->checkEnvironement(m_position, target, m_radius, intersection);
}

void fwControlThirdPerson::updateDirection(void)
{
	m_direction.x = (float)cos(m_phi);
	m_direction.z = (float)sin(m_phi);
}

/**
 * update the camera position and angle
 */
void fwControlThirdPerson::updateCamera(time_t delta)
{
	// ask the collision engine what is the altitude of the target position
	if (m_collision) {
		float z = m_collision->ground(m_position);

		if (m_freefall) {
			// manage freefall

			m_time += delta;
			glm::vec3 t(m_time * m_time / 2, m_time, 1);
			glm::vec3 target = m_physic * t;

			if (!checkCollision(target)) {
				m_position = target;
			}
			else {
				// move to fall down
				m_velocity = glm::vec3(0);
				m_time = 0;
				m_physic[0][0] = 0;			m_physic[1][0] = m_velocity.x;		m_physic[2][0] = m_position.x;
				m_physic[0][1] = -0.000981f; m_physic[1][1] = m_velocity.y;		m_physic[2][1] = m_position.y;
				m_physic[0][2] = 0;			m_physic[1][2] = m_velocity.z;		m_physic[2][2] = m_position.z;
			}

			if (m_position.y - z < 0.5f) {
				// end of freefall
				m_position.y = z + 0.5f;	// TODO : use the actor height
				m_freefall = false;
				m_velocity.y = 0;

				glm::vec3 jmp = m_position - m_debug;
				std::cerr << "fwControlThirdPerson::updateCamera z=" << z << " distance=" << glm::length(jmp) << std::endl;
			}
//			else {
//				std::cerr << "fwControlThirdPerson::update x=" << m_position.x << " y=" << m_position.y << " z=" << m_position.z << std::endl;
//			}
		}
		else {
			// only test collision & fall if we move
			if (m_position.y - z > 0.5f) {
				// detect if we move to freefall and need to engage the physic engine
				m_physic[0][0] = 0;			m_physic[1][0] = m_velocity.x;		m_physic[2][0] = m_position.x;
				m_physic[0][1] = -0.000981f; m_physic[1][1] = m_velocity.y;		m_physic[2][1] = m_position.y;
				m_physic[0][2] = 0;			m_physic[1][2] = m_velocity.z;		m_physic[2][2] = m_position.z;

				m_freefall = true;
				m_time = 0;
				std::cerr << "fwControlThirdPerson::updateCamera x=" << m_position.x << " y=" << m_position.y << " z=" << m_position.y << std::endl;
			}
			else {
				// stick to the floor
				m_position.y = z + 0.5f;	// TODO : use the actor height
				m_freefall = false;

				// and move
				glm::vec3 target = m_position + m_velocity * (float)delta;
				if (!checkCollision(target)) {
					m_position = target;
				}

				// std::cerr << "fwControlThirdPerson::updateCamera z=" << z << std::endl;
			}
		}
	}

	glm::vec3 lookAt(
		2 * cos(m_phi)*sin(m_theta) + m_position.x,
		cos(m_theta) + m_position.y,
		2 * sin(m_phi) * sin(m_theta) + m_position.z
	);

	//std::cout << m_phi << " " << tmp.x << " " << tmp.y << std::endl;

	camera->translate(m_position);
	camera->lookAt(lookAt);
}

/**
 * auto update the controler if autoupdated
 */
void fwControlThirdPerson::update(time_t delta)
{
	checkKeys(delta);

	updateCamera(delta);
}

fwControlThirdPerson::~fwControlThirdPerson()
{

}