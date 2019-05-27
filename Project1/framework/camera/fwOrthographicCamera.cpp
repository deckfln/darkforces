#include "fwOrthographicCamera.h"

fwOrthographicCamera::fwOrthographicCamera(float size, float near_plane, float far_plane)
{
	projection = glm::ortho(-size, size, -size, size, near_plane, far_plane);
}

fwOrthographicCamera::~fwOrthographicCamera()
{

}