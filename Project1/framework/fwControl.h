#pragma once

#include <math.h>
#include <time.h>
#include <glm/vec3.hpp>

#include "fwCamera.h"
#include "fwCollision.h"

const double pi = 3.1415926535897;

class fwControl
{
	virtual void _mouseMove(float xdir, float ydir) {};
	virtual void _mouseButton(int action) {};
	virtual void _mouseScroll(double xoffset, double yoffset) {};
	virtual void _keyEvent(int key, int action) {};

protected:
	bool managed = false;	// mouse is managed by the controler
	int m_button = 0;		// current mouse button pressed
	bool m_shift = false;	// shift key is pressed
	std::map<int, bool> m_currentKeys;	// current keys pressed
	std::map<int, bool> m_prevKeys;		// previous keys status

	// mouse tracking
	double m_currentX = 0;
	double m_currentY = 0;
	double m_startx = 0;
	double m_starty = 0;

	bool m_autoupdate = false;	// controller update itself without user action (like when the key is kept pressed)

	fwCamera *m_camera = nullptr;
	fwCollision* m_collision = nullptr;	// move engine

	virtual void updateCamera(time_t delta) {};

public:
	fwControl(fwCamera *);

	void mouseButton(int button, int action);
	void mouseMove(double xpos, double ypos);
	void mouseScroll(double xoffset, double yoffset);
	void keyEvent(int key, int scancode, int action, int mods);
	void bind(fwCollision* collision) { m_collision = collision; };
	virtual void update(time_t) {};
	bool isKeyPressed(int key);
	fwCamera* bindCamera(fwCamera* camera);		// bind the controller to another camera
	~fwControl();
};