#pragma once

#include <math.h>
#include "../fwCamera.h"

const double pi = 3.1415926535897;

class fwOrbitControl
{
	bool managed = false;
	double startx = 0;
	double starty = 0;
	double m_theta = pi/4;
	double m_phi = 0;
	double m_radius = 5;

	fwCamera *camera = nullptr;

public:
	fwOrbitControl(fwCamera *, float radius = 5);
	void mouseButton(int button, int action);
	void mouseMove(double xpos, double ypos);
	void mouseScroll(double xoffset, double yoffset);
	void update(void);
	double radius(void) { return m_radius; };
	void radius(double r) { m_radius = r; };
	~fwOrbitControl();
};