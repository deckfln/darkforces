#pragma once

#include "math/fwPlane.h"
#include "math/fwSphere.h"
#include "fwMesh.h"

class fwFrustum
{
	fwPlane m_planes[6];

public:
	fwFrustum();
	fwFrustum &setFromMatrix(glm::mat4 matrix);
	bool intersectsObject(fwMesh *object);
	bool intersectsSphere(fwSphere& sphere, fwSphere& source);
	~fwFrustum();
};