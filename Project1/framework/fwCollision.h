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
    fwCollision(void);

    // ground 'around" the character (floor or platform, below or slightly above for stairs)
    virtual float ground(glm::vec3& position) { return 0; };
    virtual bool wall(glm::vec3& position, glm::vec3& direction, glm::vec3& intersection) { return false; };

    ~fwCollision(void);
};