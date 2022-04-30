#pragma once

#include <math.h>
#include <time.h>
#include <glm/vec3.hpp>

#include "../fwControl.h"

class fwOrbitControl : public fwControl
{
	glm::vec3 m_lookAt = glm::vec3(0);
	glm::vec3 m_center = glm::vec3(0);
	glm::vec3 m_origLookAt = glm::vec3(0);
	glm::mat4 m_inverseCamera = glm::mat4(1);

	double m_theta = pi/4;
	double m_phi = 0;
	double m_radius = 5;

	void _mouseMove(float xdir, float ydir);
	void _mouseButton(int action);
	void _mouseScroll(double xoffset, double yoffset);
	void updateCamera(time_t delta) override;

public:
	fwOrbitControl(fwCamera *, float radius, glm::vec3 lookAt);
	double radius(void) { return m_radius; };
	void radius(double r) { m_radius = r; };
	void setFromCamera(void);					// set center & lookAt from the current camera
	void translateCamera(const glm::vec3& pos);	// move the center of the orbit camera
};