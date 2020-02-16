#include "fwControl.h"

#include <GLFW/glfw3.h>

fwControl::fwControl(fwCamera* _camera) :
	camera(_camera)
{
	update(0);
}

void fwControl::mouseButton(int button, int action)
{
	m_button = button;

	switch (button) {
	case GLFW_MOUSE_BUTTON_LEFT:
		if (action == GLFW_PRESS) {
			managed = true;
			m_startx = m_currentX;
			m_starty = m_currentY;
			_mouseButton(action);
		}
		else {
			managed = false;
			_mouseButton(action);
		}
		break;
	case GLFW_MOUSE_BUTTON_RIGHT:
		if (action == GLFW_PRESS) {
			managed = true;
			m_startx = m_currentX;
			m_starty = m_currentY;
			_mouseButton(action);
		}
		else {
			managed = false;
			_mouseButton(action);
		}
		break;
	case GLFW_MOUSE_BUTTON_MIDDLE:
		break;
	}
}

void fwControl::mouseMove(double xpos, double ypos)
{
	m_currentX = xpos;
	m_currentY = ypos;

	if (managed) {
		double xdir = 0;
		double ydir = 0;

		switch (m_button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			// find direction
			xdir = m_currentX - m_startx;
			ydir = m_currentY - m_starty;

			_mouseMove(xdir, ydir);
			break;

		case GLFW_MOUSE_BUTTON_RIGHT:
			_mouseMove(xdir, ydir);
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			break;
		}

		updateCamera();
	}
}

void fwControl::mouseScroll(double xoffset, double yoffset)
{
	_mouseScroll(xoffset, yoffset);
	updateCamera();
}

/**
 * Deal with key pressed, released to activate autoupdate
 */
void fwControl::keyEvent(int key, int scancode, int action, int mods)
{
	m_shift = mods & GLFW_MOD_SHIFT;

	switch (action) {
	case GLFW_PRESS:
		m_currentKeys[key] = true;
		break;
	case GLFW_RELEASE:
		m_currentKeys[key] = false;
		break;
	}

	// trigger autoupdate if there is at least 1 key presssed
	m_autoupdate = false;
	for (auto key : m_currentKeys) {
		if (key.second) {
			m_autoupdate = true;
			break;
		}
	}
}

bool fwControl::isKeyPressed(int key)
{
	return m_currentKeys[key];
}

fwControl::~fwControl()
{

}