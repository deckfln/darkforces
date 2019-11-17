#pragma once

#include <math.h>
#include "../fwCamera.h"

const double pi = 3.1415926535897;

class fwOrbitControl
{
	bool managed = false;
	double startx;
	double starty;
	double theta = pi/4;
	double phi = 0;
	double m_radius = 5;

	fwCamera *camera = nullptr;

	void update(void);

public:
	fwOrbitControl(fwCamera *, float radius = 5);
	void mouseButton(int button, int action);
	void mouseMove(double xpos, double ypos);
	void mouseScroll(double xoffset, double yoffset);
	~fwOrbitControl();
};