#include "fwControlThirdPerson.h"

#include <GLFW/glfw3.h>

fwControlThirdPerson::fwControlThirdPerson(fwCamera* _camera, glm::vec3 position, float height, float phi, float radius) :
	fwControl(_camera),
	m_position(position),
	m_height(height),
	m_phi(phi),
	m_radius(radius)
{
	updateDirection();
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
			m_dirty = true;
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

		// lock the camera up and down
		double t = m_theta_start + (double)ydir * m_radSpeed;
		if (t <= m_theta_lock_down || t >= m_theta_lock_up) {
			return;
		}
		m_theta = t;
		m_dirty = true;
		break;
	}
}


/**
 * deal with movement keys
 */
bool fwControlThirdPerson::checkKeys(time_t delta)
{
	float running = m_shift ? 2.0f : 1.0f;

	if (!m_locked) {
		if (m_currentKeys[GLFW_KEY_UP]) {
			m_velocity = m_direction * running;
			m_dirty = true;
		}
		if (m_currentKeys[GLFW_KEY_DOWN]) {
			m_velocity = -m_direction * running;
			m_dirty = true;
		}

		if (!m_currentKeys[GLFW_KEY_DOWN] && !m_currentKeys[GLFW_KEY_UP]) {
			m_velocity.x = m_velocity.z = 0.0f;
		}
		if (m_currentKeys[GLFW_KEY_LEFT]) {
			m_phi -= 0.003 * delta;
			m_dirty = true;
			updateDirection();
		}
		if (m_currentKeys[GLFW_KEY_RIGHT]) {
			m_phi += 0.003 * delta;
			m_dirty = true;
			updateDirection();
		}
	}

	return true;
}

/**
 * Move the player in the given direction if there is no move
 */
bool fwControlThirdPerson::checkCollision(glm::vec3& target)
{
	return false;
}

void fwControlThirdPerson::updateDirection(void)
{
	m_direction.x = (float)cos(m_phi);
	m_direction.z = (float)sin(m_phi);
}

/**
 *
 */
void fwControlThirdPerson::updatePlayer(time_t delta)
{
}

/**
 * update the camera position and angle
 */
void fwControlThirdPerson::updateCamera(time_t delta)
{
	if (m_dirty) {
		m_eye = m_position;
		m_eye.y += m_height;

		m_lookDirection = glm::vec3(
			2 * cos(m_phi) * sin(m_theta),
			cos(m_theta),
			2 * sin(m_phi) * sin(m_theta)
		);

		m_lookAt = m_lookDirection + m_eye;

		//std::cout << m_phi << " " << tmp.x << " " << tmp.y << std::endl;
		m_camera->lookAt(m_lookAt);
		m_camera->translate(m_eye);

		m_dirty = false;
	}
}

/**
 * auto update the controller if auto-updated
 */
void fwControlThirdPerson::update(time_t delta)
{
	fwControl::update(delta);
	updateCamera(delta);
	updatePlayer(delta);
}

fwControlThirdPerson::~fwControlThirdPerson()
{

}