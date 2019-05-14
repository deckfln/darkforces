#pragma once

#include "../Camera.h"

class fwOrbitControl
{
	bool managed = false;
	float startx;
	float starty;
	float theta = 0;
	float phi = 0;

	Camera *camera = nullptr;

public:
	fwOrbitControl(Camera *);
	void mouseEvent(int button, float xpos, float ypos);
	~fwOrbitControl();
};