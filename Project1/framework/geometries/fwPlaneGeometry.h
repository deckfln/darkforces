#pragma once
#include "../fwGeometry.h"

class fwPlaneGeometry :
	public fwGeometry
{
public:
	fwPlaneGeometry(float width, float height, int widthSegments=1, int heightSegments=1);
};

