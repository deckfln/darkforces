#include "fwOrbitControl.h"

#include <GLFW/glfw3.h>

fwOrbitControl::fwOrbitControl(fwCamera *_camera, float _radius, glm::vec3 lookAt):
	fwControl(_camera),
	m_radius(_radius),
	m_lookAt(lookAt)
{
	update(0);
}

void fwOrbitControl::_mouseButton(int action)
{
	switch (m_button) {
	case GLFW_MOUSE_BUTTON_LEFT:
	case GLFW_MOUSE_BUTTON_RIGHT:
		m_center = m_camera->get_position();
		m_origLookAt = m_camera->lookAt();
		m_inverseCamera = glm::inverse(m_camera->GetProjectionMatrix() * m_camera->GetViewMatrix());
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		break;
	}
}

void fwOrbitControl::_mouseScroll(double xoffset, double yoffset)
{
	if (yoffset > 0)
		m_radius -= m_radius / 10.0f;
	else 
		m_radius += m_radius / 10.0f;
}

void fwOrbitControl::_mouseMove(float xdir, float ydir)
{
	switch (m_button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		m_theta -= ydir;
		m_phi += xdir;
		m_startx = m_currentX;
		m_starty = m_currentY;
		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		// evaluate mouse movement on screen
		// and project screen to space
		glm::vec4 c((m_currentX - m_startx) * 50, (m_currentY - m_starty) * 50, -1.0, 1.0);
		c = m_inverseCamera * c;
		c.w = 1.0f / c.w;
		c.x *= c.w;
		c.y *= c.w;
		c.z *= c.w;

		// move the camera and the camera target
		glm::vec3 delta = glm::vec3(c) - m_center;
		m_lookAt = m_origLookAt + delta;

		break;
	}
}

void fwOrbitControl::updateCamera(time_t delta)
{
	float z = m_radius * cos(m_phi)*sin(m_theta) + m_lookAt.z;
	float x = m_radius * sin(m_phi)*sin(m_theta) + m_lookAt.x;
	float y = m_radius * cos(m_theta) + m_lookAt.y;

	m_camera->translate(x, y, z);
	m_camera->lookAt(m_lookAt);
}

/**
 * set center & lookAt from the current camera
 */
void fwOrbitControl::setFromCamera(void)
{
	m_center = m_camera->get_position();
	m_lookAt = m_camera->lookAt();

	glm::vec3 direction = glm::normalize(m_lookAt - m_center);
	m_lookAt = m_center;
	m_center -= direction;
	m_radius = 1.0f;
	m_theta = acos(direction.y);
	m_phi = acos(direction.z / sin(m_theta));

	updateCamera(0);
}

/*
 * move the center of the orbit camera
 */
void fwOrbitControl::translateCamera(const glm::vec3& pos)
{
	m_lookAt = m_center = pos;
	updateCamera(0);
}

fwOrbitControl::~fwOrbitControl()
{

}