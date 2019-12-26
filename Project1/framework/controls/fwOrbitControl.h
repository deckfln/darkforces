#pragma once

#include <math.h>
#include <glm/vec3.hpp>

#include "../fwCamera.h"

const double pi = 3.1415926535897;

class fwOrbitControl
{
	bool managed = false;
	int m_button = 0;

	double m_currentX = 0;
	double m_currentY = 0;

	glm::vec3 m_lookAt = glm::vec3(0);
	glm::vec3 m_camera = glm::vec3(0);
	glm::vec3 m_origLookAt = glm::vec3(0);
	glm::mat4 m_inverseCamera = glm::mat4(1);

	double startx = 0;
	double starty = 0;
	double m_theta = pi/4;
	double m_phi = 0;
	double m_radius = 5;

	fwCamera *camera = nullptr;

public:
	fwOrbitControl(fwCamera *, float radius, glm::vec3 lookAt);
	void mouseButton(int button, int action);
	void mouseMove(double xpos, double ypos);
	void mouseScroll(double xoffset, double yoffset);
	void update(void);
	double radius(void) { return m_radius; };
	void radius(double r) { m_radius = r; };
	~fwOrbitControl();
};