/*********************
 *
 * Collision Engine super class
 *
 ********************/

#pragma once

#include <glm/vec3.hpp>

enum class fwCollisionLocation {
    COLLIDE,
    NONE,
    TOP,
    BOTTOM,
    RIGHT,
    RIGHT_BOTTOM,
    RIGHT_TOP,
    LEFT,
    LEFT_BOTTOM,
    LEFT_TOP,
    FRONT,
    FRONT_BOTTOM,
    FRONT_TOP,
    BACK,
    BACK_BOTTOM,
    BACK_TOP,
};

class fwCollisionPoint 
{
public:
    fwCollisionLocation m_location = fwCollisionLocation::NONE;
	glm::vec3 m_position = glm::vec3(0);

    fwCollisionPoint(fwCollisionLocation location, const glm::vec3& position) {
        m_location = location;
        m_position = position;
    }
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

namespace Framework 
{
    bool IntersectSegmentTriangle(const glm::vec3& p,
        const glm::vec3& q,
        const glm::vec3& a,
        const glm::vec3& b,
        const glm::vec3& c,
        float& u,
        float& v,
        float& w,
        float& t);

    bool lineSegIntersectTri(
        const glm::vec3& p,
        const glm::vec3& q,
        const glm::vec3& a,
        const glm::vec3& b,
        const glm::vec3& c,
        glm::vec3& point);

    /**
     * Sphere/Triangle intersection (other version)
     */
    bool intersectSphereTriangle(
        const glm::vec3& center_es,
        const glm::vec3& a, const glm::vec3 b, const glm::vec3 c,
        glm::vec3& p);

    /*
     * Real-Time Collision Detection
     */
    bool IntersectLineTriangle(
        const glm::vec3& p, const glm::vec3& q, 
        const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
        glm::vec3& collision);
}
