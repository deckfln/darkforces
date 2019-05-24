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

	fwCamera *camera = nullptr;

public:
	fwOrbitControl(fwCamera *);
	void mouseButton(int button, int action);
	void mouseMove(float xpos, float ypos);
	~fwOrbitControl();
};