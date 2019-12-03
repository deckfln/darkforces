#include "fwOrbitControl.h"

#include <GLFW/glfw3.h>

fwOrbitControl::fwOrbitControl(fwCamera *_camera, float _radius):
	camera(_camera),
	m_radius(_radius)
{
	update();
}

void fwOrbitControl::mouseButton(int button, int action)
{
	m_button = button;

	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			managed = true;
			startx = starty = -1;
		}
		else {
			managed = false;
		}
		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS) {
			managed = true;
			startx = m_currentX;
			starty = m_currentY;
			m_camera = camera->get_position();
			m_origLookAt = camera->lookAt();
			m_inverseCamera = glm::inverse(camera->GetProjectionMatrix() * camera->GetViewMatrix());
		}
		else {
			managed = false;
		}
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		break;
	}
}

void fwOrbitControl::mouseScroll(double xoffset, double yoffset)
{
	m_radius -= yoffset;
	update();
}

void fwOrbitControl::mouseMove(double xpos, double ypos)
{
	m_currentX = xpos;
	m_currentY = ypos;

	if (managed) {
		double xdir = 0;
		double ydir = 0;

		switch (m_button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			if (startx != -1) {
				// find direction
				xdir = xpos - startx;
				ydir = ypos - starty;
			}
			m_theta += ydir;
			m_phi += xdir;

			startx = xpos;
			starty = ypos;
			break;

		case GLFW_MOUSE_BUTTON_RIGHT:
			// evaluate mouse movement on screen
			// and project screen to space

			glm::vec4 c((xpos - startx) * 50, (ypos - starty) * 50, -1.0, 1.0);
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

		update();

	}
}

void fwOrbitControl::update(void)
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