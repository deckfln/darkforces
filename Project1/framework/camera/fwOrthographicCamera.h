#pragma once

#include "../fwCamera.h"

class fwOrthographicCamera : public fwCamera
{
public:
	fwOrthographicCamera(float size, float near_plane, float far_plane);
	~fwOrthographicCamera();
};