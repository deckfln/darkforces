#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "../fwControl.h"

class fwControlThirdPerson : public fwControl
{
	double m_radSpeed = pi / 2;			// map mouse move 0..1 to 0..pi/2
	double m_theta = pi / 2;			// current up/down angle
	double m_phi = -pi / 2;				// current left/right angle

	double m_theta_start = 0;			// value when the mouse was clicked
	double m_phi_start = 0;

	double m_theta_lock_down = 0;		// minimum and maximum value to lock down and up
	double m_theta_lock_up = M_PI;

	float m_speed = 0.015f;
	float m_height = 1;					// player eye position
	float m_radius = 1;					// player cylinder radius 

	glm::mat3 m_physic = glm::mat3(0);	// matrices for physic engine when in 'free fall'
	time_t m_animation_time = 0;

	glm::vec3 m_debug;			// start of jump for debug

	void _mouseMove(float xdir, float ydir);
	void _mouseButton(int action);
	void updateDirection(void);
	bool checkCollision(glm::vec3& target);

protected:
	bool m_locked = false;						// block movement (like in a free-fall)

	glm::vec3 m_direction = glm::vec3(0);
	glm::vec3 m_position = glm::vec3(0);
	glm::vec3 m_velocity = glm::vec3(0);
	glm::vec3 m_lookDirection = glm::vec3(0);	// direction of the look
	glm::vec3 m_lookAt = glm::vec3(0);			// same but origin the eyes
	glm::vec3 m_eye = glm::vec3(0);
	bool m_dirty = true;					// dirty when the player moves

	virtual bool checkKeys(time_t delta);
	virtual void updatePlayer(time_t delta);

public:
	fwControlThirdPerson(fwCamera *, glm::vec3 position, float height, float phi, float radius);
	void update(time_t delta);
	void lockView(double down, double up) { m_theta_lock_down = down; m_theta_lock_up = up; };
	void keyEvent(int key, int scancode, int action);
	void updateCamera(time_t delta);

	// getter/setter
	inline const glm::vec3& direction(void) { return m_direction; };
	inline const glm::vec3& lookAt(void) { return m_lookAt; };
	inline const glm::vec3& position(void) { return m_position; };

	~fwControlThirdPerson();
};