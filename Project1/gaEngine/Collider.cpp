#include "Collider.h"

#include <glm/gtx/intersect.hpp>

#include "../framework/fwAABBox.h"
#include "../framework/fwGeometry.h"
#include "../framework/fwCollision.h"

#include "AABBoxTree.h"

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
 * collider based on a gaAABB (split triangles by AABB)
 */
void GameEngine::Collider::set(AABBoxTree* modelAABB, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix)
{
	m_type = ColliderType::AABB_TREE;
	m_aabbTree = modelAABB;
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
	else if (m_type == ColliderType::AABB && source.m_type == ColliderType::AABB_TREE) {
		return collision_fwAABB_gaAABB(*this, source, forward, down, collisions);
	}
	else if (m_type == ColliderType::AABB_TREE && source.m_type == ColliderType::AABB) {
		return collision_fwAABB_gaAABB(source, *this, forward, down, collisions);
	}
	else if (m_type == ColliderType::AABB_TREE && source.m_type == ColliderType::AABB_TREE) {
		printf("Collider::collision GA_AABB vs GA_AABB not implemented");
		exit(-1);
	}
	else if (m_type == ColliderType::AABB && source.m_type == ColliderType::AABB) {
		return true;	// the worldAABB already collide, no need for further test
	}
	return false;
}

/**
 * doing AABB vs geometry collision
 * test all triangles of the geometry
 */
bool Collider::collisionAABBgeometry(const Collider& aabb, 
	const Collider& geometry, 
	const glm::vec3& forward, // in world space
	const glm::vec3& down,
	std::list<gaCollisionPoint>& collisions)
{
	glm::vec3 const* vertices = geometry.m_geometry->vertices();
	uint32_t nbVertices = geometry.m_geometry->nbvertices();

	float u, v, w;

	// move the AABB from model space to worldSpace and then to geometry model space
	glm::mat4 mat = *geometry.m_inverseWorldMatrix * *aabb.m_worldMatrix;
	fwAABBox aabb_geometry_space(aabb.m_aabb, mat);

	// remove the translation part, this is a direction vector
	glm::mat4 rotation_scale = *geometry.m_inverseWorldMatrix;
	rotation_scale[3][0] = rotation_scale[3][1] = rotation_scale[3][2] = 0;
	glm::vec3 forward_geometry_space = glm::vec3(rotation_scale * glm::vec4(forward, 1.0));

	// half of the length of the source AABB
	float half_height = (aabb.m_aabb->m_p1.y - aabb.m_aabb->m_p.y) / 2.0f;
	glm::vec3 center = glm::vec3(mat * glm::vec4(0, 0, 0, 1));
	glm::vec3 fwsensor = center + forward_geometry_space;		// forward sensor
	glm::vec3 dwsensor = down * half_height;					// downward sensor

	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	fwAABBox triangle;
	glm::vec3 collision;

	for (unsigned int i = 0; i < nbVertices; i += 3) {
		triangle.set(vertices + i, 3);

		if (triangle.intersect(aabb_geometry_space)) {

			// check the forward sensor
			if (glm::intersectLineTriangle(center,
				forward_geometry_space,
				vertices[i], vertices[i + 1], vertices[i + 2],
				collision))
			{
				u = collision.x;
				v = collision.y;
				w = 1 - (u + v);

				collision = (u * vertices[i] + v * vertices[i + 1] + w * vertices[i + 2]);

				// rebuild collision point (geometry space) to world space 
				//collision = u * vertices[i] + v * vertices[i + 1] + w * vertices[i + 2];
				collision = glm::vec3(*geometry.m_worldMatrix * glm::vec4(collision, 1.0));
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::FRONT, collision, nullptr));
			};

			// check the downward sensor
			if (glm::intersectLineTriangle(center,
				dwsensor,
				vertices[i], vertices[i + 1], vertices[i + 2],
				collision)) 
			{
			u = collision.x;
			v = collision.y;
			w = 1 - (u + v);

			collision = (u * vertices[i] + v * vertices[i + 1] + w * vertices[i + 2]);

			collision = glm::vec3(*geometry.m_worldMatrix * glm::vec4(collision, 1.0));
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::BOTTOM, collision, nullptr));
			};
		}
	}

	return collisions.size() != 0;
}

/**
 * do a fwAABB vs gaAABB collision check
 * only test triangles included in the smallest gaAABB hierarchy
 */
bool GameEngine::Collider::collision_fwAABB_gaAABB(const Collider& fwAABB, 
	const Collider& gaAABB, 
	const glm::vec3& forward, 
	const glm::vec3& down, 
	std::list<gaCollisionPoint>& collisions)
{
	float u, v, w;

	// move the AABB from model space to worldSpace and then to geometry model space
	glm::mat4 mat = *gaAABB.m_inverseWorldMatrix * *fwAABB.m_worldMatrix;
	fwAABBox aabb_geometry_space(fwAABB.m_aabb, mat);

	// remove the translation part, this is a direction vector
	glm::mat4 rotation_scale = *gaAABB.m_inverseWorldMatrix;
	rotation_scale[3][0] = rotation_scale[3][1] = rotation_scale[3][2] = 0;
	glm::vec3 forward_geometry_space = glm::vec3(rotation_scale * glm::vec4(forward, 1.0));

	// half of the length of the source AABB
	float half_height = (fwAABB.m_aabb->m_p1.y - fwAABB.m_aabb->m_p.y) / 2.0f;
	glm::vec3 center = glm::vec3(mat * glm::vec4(0, 0, 0, 1));
	glm::vec3 fwsensor = center + forward_geometry_space;		// forward sensor
	glm::vec3 dwsensor = down * half_height;					// downward sensor

	// find the smallest set of gaAABB intersecting with the fwAABB
	// and test only the included triangles
	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	fwAABBox triangle;
	glm::vec3 collision;

	std::vector<GameEngine::AABBoxTree*> hits;
	if (!gaAABB.m_aabbTree->find(aabb_geometry_space, hits)) {
		return false;
	}

	glm::vec3 const* vertices; 
	uint32_t nbVertices;

	for (auto aabb : hits) {
		vertices = aabb->vertices();
		nbVertices = aabb->nbVertices();

		for (unsigned int i = 0; i < nbVertices; i += 3) {
			triangle.set(vertices + i, 3);

			if (triangle.intersect(aabb_geometry_space)) {

				// check the forward sensor
				if (glm::intersectLineTriangle(center,
					forward_geometry_space,
					vertices[i], vertices[i + 1], vertices[i + 2],
					collision))
				{
					u = collision.x;
					v = collision.y;
					w = 1 - (u + v);

					collision = (u * vertices[i] + v * vertices[i + 1] + w * vertices[i + 2]);

					// rebuild collision point (geometry space) to world space 
					//collision = u * vertices[i] + v * vertices[i + 1] + w * vertices[i + 2];
					collision = glm::vec3(*gaAABB.m_worldMatrix * glm::vec4(collision, 1.0));
					collisions.push_back(gaCollisionPoint(fwCollisionLocation::FRONT, collision, nullptr));
				};

				// check the downward sensor
				if (glm::intersectLineTriangle(center,
					dwsensor,
					vertices[i], vertices[i + 1], vertices[i + 2],
					collision))
				{
					u = collision.x;
					v = collision.y;
					w = 1 - (u + v);

					collision = (u * vertices[i] + v * vertices[i + 1] + w * vertices[i + 2]);

					collision = glm::vec3(*gaAABB.m_worldMatrix * glm::vec4(collision, 1.0));
					collisions.push_back(gaCollisionPoint(fwCollisionLocation::BOTTOM, collision, nullptr));
				};
			}
		}
	}

	return collisions.size() != 0;
}
