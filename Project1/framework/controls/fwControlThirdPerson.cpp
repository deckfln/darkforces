#include "fwControlThirdPerson.h"

#include <GLFW/glfw3.h>

fwControlThirdPerson::fwControlThirdPerson(fwCamera* _camera, glm::vec3 position, glm::vec3 direction, float radius) :
	fwControl(_camera),
	m_position(position),
	m_direction(direction),
	m_radius(radius)
{
	updateCamera();
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
	if (m_currentKeys[GLFW_KEY_UP]) {
		checkCollision(1, delta);
	}
	if (m_currentKeys[GLFW_KEY_DOWN]) {
		checkCollision(-1, delta);
	}
	if (m_currentKeys[GLFW_KEY_LEFT]) {
		m_phi -= 0.003 * delta;
		updateDirection();
	}
	if (m_currentKeys[GLFW_KEY_RIGHT]) {
		m_phi += 0.003 * delta;
		updateDirection();
	}
}

/**
 * Move the player in the given sirection if there is no collision
 */
void fwControlThirdPerson::checkCollision(float sign, time_t delta)
{
	glm::vec3 direction = m_direction * (m_speed * delta) * sign;
	glm::vec3 target = m_position + direction;
	glm::vec3 intersection;

	if (!m_collision->checkEnvironement(m_position, target, m_radius, intersection)) {
		m_position += direction;
	}
	else {
		// collision, check if the collision point is 'behind' the direction vector
		glm::vec3 AC = intersection - m_position;
		float d = glm::dot(direction, AC);
		if (d <= 0) {
			// the collision point is on the other side of the direction
			m_position += direction;
		}
	}
	updateCamera();
}

void fwControlThirdPerson::updateDirection(void)
{
	m_direction.x = (float)cos(m_phi);
	m_direction.z = (float)sin(m_phi);
	updateCamera();
}

/**
 * update the camera position and angle
 */
void fwControlThirdPerson::updateCamera(void)
{
	// ask the collision engine what is the altitude of the target position
	if (m_collision) {
		float z = m_collision->ground(m_position);
		m_position.y = z + 0.5f;	// TODO : use the actor height
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
	if (m_autoupdate) {
		checkKeys(delta);
	}
}

fwControlThirdPerson::~fwControlThirdPerson()
{

}