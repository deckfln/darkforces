#include "fwOrbitControl.h"

#include <GLFW/glfw3.h>

fwOrbitControl::fwOrbitControl(fwCamera *_camera, float _radius):
	camera(_camera),
	m_radius(_radius)
{

}

void fwOrbitControl::mouseButton(int button, int action)
{
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
	if (managed) {
		double xdir = 0;
		double ydir = 0;

		if (startx != -1) {
			// find direction
			xdir = xpos - startx ;
			ydir = ypos - starty;
		}
		theta += ydir;
		phi += xdir;

		update();

		startx = xpos;
		starty = ypos;
	}
}

void fwOrbitControl::update(void)
{
	float z = m_radius * cos(phi)*sin(theta);
	float x = m_radius * sin(phi)*sin(theta);
	float y = m_radius * cos(theta);

	camera->translate(x, y, z);
}

fwOrbitControl::~fwOrbitControl()
{

}