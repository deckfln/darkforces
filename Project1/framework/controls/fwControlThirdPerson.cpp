#include "fwControlThirdPerson.h"

#include <GLFW/glfw3.h>

fwControlThirdPerson::fwControlThirdPerson(fwCamera* _camera, glm::vec3 position, glm::vec3 direction) :
	fwControl(_camera),
	m_position(position),
	m_direction(direction)
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
void fwControlThirdPerson::checkKeys(void)
{
	if (m_currentKeys[GLFW_KEY_UP]) {
		m_position += m_direction;
		updateCamera();
	}
	if (m_currentKeys[GLFW_KEY_DOWN]) {
		m_position -= m_direction;
		updateCamera();
	}
	if (m_currentKeys[GLFW_KEY_LEFT]) {
		m_phi -= 0.1;
		updateDirection();
	}
	if (m_currentKeys[GLFW_KEY_RIGHT]) {
		m_phi += 0.1;
		updateDirection();
	}
}

void fwControlThirdPerson::updateDirection(void)
{
	//TODO the direction is normalized, speed should be handled somewhere else
	m_direction.x = cos(m_phi) / 3;
	m_direction.z = sin(m_phi) / 3;
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
		m_position.y = z + 0.5;	// TODO : use the actor height
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
void fwControlThirdPerson::update(void)
{
	if (m_autoupdate) {
		checkKeys();
	}
}

fwControlThirdPerson::~fwControlThirdPerson()
{

}