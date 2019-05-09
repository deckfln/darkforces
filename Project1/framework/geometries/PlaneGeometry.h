#pragma once
#include "framework/Geometry.h"

class PlaneGeometry :
	public Geometry
{
public:
	PlaneGeometry(float width, float height, int widthSegments=1, int heightSegments=1);
	~PlaneGeometry();
};

