#include "fwOrbitControl.h"

#include <GLFW/glfw3.h>

fwOrbitControl::fwOrbitControl(Camera *_camera):
	camera(_camera)
{

}

void fwOrbitControl::mouseEvent(int button, float xpos, float ypos)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (!managed) {
			startx = xpos;
			starty = ypos;
			managed = true;
		}
	}
	else {
		managed = false;
	}

	if (managed) {
		float radius = 5;

		// find direction
		float xdir = xpos - startx;
		float ydir = ypos - starty;

		theta += xdir* 0.1;
		phi += ydir * 0.1;

		float x = radius * cos(phi)*sin(theta);
		float y = radius * sin(phi)*sin(theta);
		float z = radius * cos(theta);

		camera->translate(x, y, z);
	}
}

fwOrbitControl::~fwOrbitControl()
{

}