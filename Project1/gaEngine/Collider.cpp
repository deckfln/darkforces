#include "Collider.h"

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
bool Collider::collision(const Collider& source, const glm::vec3& direction, glm::vec3& collision)
{
	if (source.m_type == ColliderType::AABB && m_type == ColliderType::GEOMETRY) {
		return collisionAABBgeometry(source, *this, direction, collision);
	}
	else if (m_type == ColliderType::AABB && source.m_type == ColliderType::GEOMETRY) {
		return collisionAABBgeometry(*this, source, direction, collision);
	}
	else if (m_type == ColliderType::AABB && source.m_type == ColliderType::AABB) {
		return true;	// the worldAABB already collide, no need for further test
	}
	return false;
}

/**
 * doing AABB vs geometry collision
 */
bool Collider::collisionAABBgeometry(const Collider& aabb, const Collider& geometry, const glm::vec3& direction, glm::vec3& collision)
{
	glm::vec3 const* vertices = geometry.m_geometry->vertices();
	uint32_t nbVertices = geometry.m_geometry->nbvertices();

	float u, v, w, t;

	// move the AABB from model space to worldSpace and then to geometry model space
	glm::mat4 mat = *geometry.m_inverseWorldMatrix * *aabb.m_worldMatrix;
	fwAABBox aabb_geometry_space(aabb.m_aabb, mat);
	aabb_geometry_space += direction;

	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	fwAABBox triangle;

	for (unsigned int i = 0; i < nbVertices; i += 3) {
		triangle.set(vertices + i, 3);

		if (triangle.intersect(aabb_geometry_space)) {
			// TODO: improve an estimated collision point
			collision = (triangle.center() + aabb_geometry_space.center()) / 2.0f;
			collision = *geometry.m_worldMatrix * glm::vec4(collision, 1.0);
			return true;
		}
		/*
		for (auto& p : points) {
			glm::vec3 q = p + direction;

			if (IntersectSegmentTriangle(p, q,
				vertices[i], vertices[i + 1], vertices[i + 2],
				u, v, w, t
			)) {
				// rebuild collision point (geometry space) to world space 
				collision = u * vertices[i] + v * vertices[i + 1] + w * vertices[i + 2];
				collision = glm::vec3(*geometry.m_worldMatrix * glm::vec4(collision, 1.0));
				return true;
			};
		}
		*/
	}

	return false;
}
