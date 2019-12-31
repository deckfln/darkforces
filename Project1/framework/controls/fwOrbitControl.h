#pragma once

#include <math.h>
#include <glm/vec3.hpp>

#include "../fwControl.h"

class fwOrbitControl : fwControl
{
	glm::vec3 m_lookAt = glm::vec3(0);
	glm::vec3 m_camera = glm::vec3(0);
	glm::vec3 m_origLookAt = glm::vec3(0);
	glm::mat4 m_inverseCamera = glm::mat4(1);

	double m_theta = pi/4;
	double m_phi = 0;
	double m_radius = 5;

	void _mouseMove(float xdir, float ydir);
	void _mouseButton(int action);
	void _mouseScroll(double xoffset, double yoffset);
	void updateCamera(void);

public:
	fwOrbitControl(fwCamera *, float radius, glm::vec3 lookAt);
	double radius(void) { return m_radius; };
	void radius(double r) { m_radius = r; };
	~fwOrbitControl();
};