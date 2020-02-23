#pragma once

#include <math.h>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "../fwControl.h"

class fwControlThirdPerson : public fwControl
{
	glm::vec3 m_direction = glm::vec3(0);
	glm::vec3 m_position = glm::vec3(0);
	glm::vec3 m_camera = glm::vec3(0);
	glm::vec3 m_origLookAt = glm::vec3(0);
	glm::mat4 m_inverseCamera = glm::mat4(1);

	double m_radSpeed = pi / 2;	// map mouse move 0..1 to 0..pi/2
	double m_theta = pi/2;
	double m_phi = -pi/2;
	double m_theta_start = 0;
	double m_phi_start = 0;
	float m_speed = 0.0015f;
	float m_radius = 1;			// radius of the player bounding Sphere

	glm::mat3 m_physic = glm::mat3(0);	// matrice for physic engine when in 'free fall'
	glm::vec3 m_velocity = glm::vec3(0);
	bool m_freefall = false;
	time_t m_time = 0;

	glm::vec3 m_debug;			// start of jump for debug

	void _mouseMove(float xdir, float ydir);
	void _mouseButton(int action);
	void updateDirection(void);
	void checkKeys(time_t delta);
	bool checkCollision(glm::vec3& target);

public:
	fwControlThirdPerson(fwCamera *, glm::vec3 position, float phi, float radius);
	void update(time_t delta);
	void keyEvent(int key, int scancode, int action);
	void updateCamera(time_t delta);
	~fwControlThirdPerson();
};