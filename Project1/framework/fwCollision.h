/*********************
 *
 * Collision Engine super class
 *
 ********************/

#pragma once

#include <glm/vec3.hpp>

class fwCollision
{
public:
    fwCollision();

    // ground 'around" the character (floor or platform, below or slightly above for stairs)
    virtual float ground(glm::vec3& position) { return 0; };
    virtual bool checkEnvironement(glm::vec3& position, glm::vec3& target, float radius, glm::vec3& intersection) { return false; };

    ~fwCollision(void);
};