#pragma once

#include <math.h>
#include "../fwCamera.h"

const float pi = 3.1415926535897;

class fwOrbitControl
{
	bool managed = false;
	float startx;
	float starty;
	float theta = pi/4;
	float phi = 0;
	float m_radius = 5;

	fwCamera *camera = nullptr;

	void update(void);

public:
	fwOrbitControl(fwCamera *, float radius = 5);
	void mouseButton(int button, int action);
	void mouseMove(float xpos, float ypos);
	void mouseScroll(float xoffset, float yoffset);
	~fwOrbitControl();
};