/*********************
 *
 * Collision Engine super class
 *
 ********************/

#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "fwAABBox.h"
#include "math/fwPlane.h"

enum class fwCollisionLocation {
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
    COLLIDE,
    WARP
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
    enum class Test {
        WITH_BORDERS,       // includes the borders of the objects
        WITHOUT_BORDERS     // only in-between (based on EPSILON)
    };

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
        fwCollision::Test test,
        glm::vec3& point);

    /**
     * Sphere/satNavTriangle intersection (other version)
     */
    bool intersectSphereTriangle(
        const glm::vec3& center_es,
        const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
        glm::vec3& p);
    bool intersectSphereOriginTriangle(
        const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
        glm::vec3& p);
    /*
     * Real-Time Collision Detection
     */
    bool IntersectLineTriangle(
        const glm::vec3& p, const glm::vec3& q, 
        const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
        glm::vec3& collision);

    // http://realtimecollisiondetection.net/blog/?p=103
    bool IntersectLineTriangle(
        const glm::vec3& p1,
        const glm::vec3& p2,
        const glm::vec3& p3,
        const glm::vec3& P,
        float r);

    bool IntersectionSphereLine(const glm::vec3& center,
        const float radius,
        const glm::vec3& pt0,
        const glm::vec3& pt1,
        glm::vec3& collision);

    bool segment2segment(glm::vec2& A, glm::vec2& B, glm::vec2& C, glm::vec2& D, glm::vec2& result);
    bool fat_point_collides_segment(glm::vec2& A, glm::vec2& B, glm::vec2& C, float r, glm::vec2& result);
    bool CircLine(glm::vec2& A, glm::vec2& B, glm::vec2& C, float r, glm::vec2& result);

    bool intersectRayAABox2(const glm::vec3& p0, const glm::vec3& p1, const fwAABBox& box, float& tnear, float& tfar);
    bool PlaneAABB(const fwAABBox& aabb, const fwPlane& plane);
    bool TriangleAABB(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const fwAABBox& aabb);
}
