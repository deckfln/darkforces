#include "fwOrbitControl.h"

#include <GLFW/glfw3.h>

fwOrbitControl::fwOrbitControl(Camera *_camera):
	camera(_camera)
{

}

void fwOrbitControl::mouseButton(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			managed = true;
			startx = starty = -1;
		}
		else {
			managed = false;
		}
	}
}

void fwOrbitControl::mouseMove(float xpos, float ypos)
{
	if (managed) {
		float xdir = 0;
		float ydir = 0;

		if (startx != -1) {
			// find direction
			xdir = xpos - startx ;
			ydir = ypos - starty;
		}
		float radius = 5;


		theta += ydir;
		phi += xdir;

		float z = radius * cos(phi)*sin(theta);
		float x = radius * sin(phi)*sin(theta);
		float y = radius * cos(theta);

		camera->translate(x, y, z);

		startx = xpos;
		starty = ypos;
	}
}

fwOrbitControl::~fwOrbitControl()
{

}