#include "Collider.h"

#include <glm/gtx/intersect.hpp>

#include "../framework/fwAABBox.h"
#include "../framework/fwGeometry.h"
#include "../framework/fwCollision.h"

using namespace GameEngine;

/**
 * Initialize a AABBox as collider
 */
Collider::Collider(fwAABBox* aabb, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix):
	m_type(ColliderType::AABB),
	m_aabb(aabb),
	m_worldMatrix(worldMatrix),
	m_inverseWorldMatrix(inverseWorldMatrix)
{
}

/**
 * Initialize a geometry as collider
 */
GameEngine::Collider::Collider(fwGeometry* geometry, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix) :
	m_type(ColliderType::GEOMETRY),
	m_geometry(geometry),
	m_worldMatrix(worldMatrix),
	m_inverseWorldMatrix(inverseWorldMatrix)
{
}

void GameEngine::Collider::set(fwGeometry* geometry, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix)
{
	m_type = ColliderType::GEOMETRY;
	m_geometry = geometry;
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = inverseWorldMatrix;
}

void GameEngine::Collider::set(fwAABBox* modelAABB, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix)
{
	m_type = ColliderType::AABB;
	m_aabb = modelAABB;
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = inverseWorldMatrix;
}

/**
 * check 2 colliders after a successful worldAABB collision detection
 */
bool Collider::collision(const Collider& source, 
	const glm::vec3& forward,
	const glm::vec3& down,
	std::list<gaCollisionPoint>& collisions)
{
	if (source.m_type == ColliderType::AABB && m_type == ColliderType::GEOMETRY) {
		return collisionAABBgeometry(source, *this, forward, down, collisions);
	}
	else if (m_type == ColliderType::AABB && source.m_type == ColliderType::GEOMETRY) {
		return collisionAABBgeometry(*this, source, forward, down, collisions);
	}
	else if (m_type == ColliderType::AABB && source.m_type == ColliderType::AABB) {
		return true;	// the worldAABB already collide, no need for further test
	}
	return false;
}

/**
 * doing AABB vs geometry collision
 */
bool Collider::collisionAABBgeometry(const Collider& aabb, 
	const Collider& geometry, 
	const glm::vec3& forward, // in world space
	const glm::vec3& down,
	std::list<gaCollisionPoint>& collisions)
{
	glm::vec3 const* vertices = geometry.m_geometry->vertices();
	uint32_t nbVertices = geometry.m_geometry->nbvertices();

	float u, v, w, t;

	// move the AABB from model space to worldSpace and then to geometry model space
	glm::mat4 mat = *geometry.m_inverseWorldMatrix * *aabb.m_worldMatrix;
	fwAABBox aabb_geometry_space(aabb.m_aabb, mat);
	glm::vec3 forward_geometry_space = glm::vec3(*geometry.m_inverseWorldMatrix * glm::vec4(forward, 1.0));

	// half of the length of the source AABB
	float half_len = aabb.m_aabb->m_p1.z - aabb.m_aabb->m_p.z;
	float half_height = (aabb.m_aabb->m_p1.y - aabb.m_aabb->m_p.y) / 2.0f;
	glm::vec3 center = glm::vec3(mat * glm::vec4(0, 0, 0, 1));
	glm::vec3 fwsensor = center + forward_geometry_space;		// forward sensor
	glm::vec3 dwsensor = down * half_height;			// downward sensor

	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	fwAABBox triangle;
	glm::vec3 collision;

	for (unsigned int i = 0; i < nbVertices; i += 3) {
		triangle.set(vertices + i, 3);

		if (triangle.intersect(aabb_geometry_space)) {
			// got a collision, now check each sensor

			if (glm::intersectLineTriangle(center,
				forward_geometry_space,
				vertices[i], vertices[i + 1], vertices[i + 2],
				collision))
			{
				u = collision.x;
				v = collision.y;
				w = 1 - (u + v);

				collision.x = (u * vertices[i].x + v * vertices[i + 1].x + w * vertices[i + 2].x);
				collision.y = (u * vertices[i].y + v * vertices[i + 1].y + w * vertices[i + 2].y);
				collision.z = (u * vertices[i].z + v * vertices[i + 1].z + w * vertices[i + 2].z);

				// rebuild collision point (geometry space) to world space 
				//collision = u * vertices[i] + v * vertices[i + 1] + w * vertices[i + 2];
				collision = glm::vec3(*geometry.m_worldMatrix * glm::vec4(collision, 1.0));
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::FRONT, collision, nullptr));
			};

			if (glm::intersectLineTriangle(center,
				dwsensor,
				vertices[i], vertices[i + 1], vertices[i + 2],
				collision)) 
			{
			u = collision.x;
			v = collision.y;
			w = 1 - (u + v);

			collision.x = (u * vertices[i].x + v * vertices[i+1].x + w * vertices[i+2].x);
			collision.y = (u * vertices[i].y + v * vertices[i+1].y + w * vertices[i+2].y);
			collision.z = (u * vertices[i].z + v * vertices[i+1].z + w * vertices[i+2].z);

			collision = glm::vec3(*geometry.m_worldMatrix * glm::vec4(collision, 1.0));
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::BOTTOM, collision, nullptr));
			};
		}
	}

	return collisions.size() != 0;
}
