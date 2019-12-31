#include "fwOrbitControl.h"

#include <GLFW/glfw3.h>

fwOrbitControl::fwOrbitControl(fwCamera *_camera, float _radius, glm::vec3 lookAt):
	fwControl(_camera),
	m_radius(_radius),
	m_lookAt(lookAt)
{
	update();
}

void fwOrbitControl::_mouseButton(int action)
{
	switch (m_button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		m_camera = camera->get_position();
		m_origLookAt = camera->lookAt();
		m_inverseCamera = glm::inverse(camera->GetProjectionMatrix() * camera->GetViewMatrix());
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		break;
	}
}

void fwOrbitControl::_mouseScroll(double xoffset, double yoffset)
{
	m_radius -= yoffset;
}

void fwOrbitControl::_mouseMove(float xdir, float ydir)
{
	switch (m_button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		m_theta += ydir;
		m_phi += xdir;
		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		// evaluate mouse movement on screen
		// and project screen to space
		glm::vec4 c((m_currentX - m_startx) * 50, (m_currentY - m_starty) * 50, -1.0, 1.0);
		c = m_inverseCamera * c;
		c.w = 1.0 / c.w;
		c.x *= c.w;
		c.y *= c.w;
		c.z *= c.w;

		// move the camera and the camera target
		glm::vec3 delta = glm::vec3(c) - m_camera;
		m_lookAt = m_origLookAt + delta;
		break;
	}
}

void fwOrbitControl::updateCamera(void)
{
	float z = m_radius * cos(m_phi)*sin(m_theta) + m_lookAt.z;
	float x = m_radius * sin(m_phi)*sin(m_theta) + m_lookAt.x;
	float y = m_radius * cos(m_theta) + m_lookAt.y;

	camera->translate(x, y, z);
	camera->lookAt(m_lookAt);
}

fwOrbitControl::~fwOrbitControl()
{

}