/*********************
 *
 * Collision Engine super class
 *
 ********************/

#pragma once

#include <glm/vec3.hpp>

enum fwCollisionPoint {
	NONE = 0,
	TOP = 1,
	BOTTOM = 2,
	RIGHT = 4,
	LEFT = 8,
	FRONT = 16,
	BACK = 32
};

class fwCollision
{
public:
    fwCollision();

    // ground 'around" the character (floor or platform, below or slightly above for stairs)
    virtual float ground(glm::vec3& position) { return 0; };
    virtual bool checkEnvironement(glm::vec3& position, glm::vec3& target, float height, float radius, glm::vec3& intersection) { return false; };

    ~fwCollision(void);
};