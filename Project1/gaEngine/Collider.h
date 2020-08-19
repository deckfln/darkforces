#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <list>

#include "gaCollisionPoint.h"

class fwAABBox;
class fwGeometry;

namespace GameEngine
{
	class AABBoxTree;

	enum class ColliderType {
		NONE,
		AABB,
		ELIPSOID,
		GEOMETRY,
		AABB_TREE
	};

	class Collider
	{
		ColliderType m_type=ColliderType::NONE;

		const glm::mat4 *m_worldMatrix = nullptr;
		const glm::mat4 *m_inverseWorldMatrix = nullptr;

		fwAABBox *m_aabb=nullptr;				// using a Framework AABB
		fwGeometry *m_geometry=nullptr;			// using a Framework geometry
		AABBoxTree* m_aabbTree = nullptr;				// using a GameEngine AABB

		bool collisionAABBgeometry(const Collider& aabb, 
			const Collider& geometry, 
			const glm::vec3& forward,
			const glm::vec3& down,
			std::list<gaCollisionPoint>& collisions);		// do a fwAABB vs geometry collision check (test all geometry triangles)

		bool collision_fwAABB_gaAABB(const Collider& fwAABB,
			const Collider& gaAABB,
			const glm::vec3& forward,
			const glm::vec3& down,
			std::list<gaCollisionPoint>& collisions);		// do a fwAABB vs gaAABB collision check (test only subset of triangles)


	public:
		Collider(void) {};									// empty collider
		Collider(fwAABBox* aabb, 
			glm::mat4* worldMatrix, 
			glm::mat4* inverseWorldMatrix);
		Collider(fwGeometry* geometry, 
			glm::mat4* worldMatrix, 
			glm::mat4* inverseWorldMatrix);
		void set(fwGeometry* geometry,
			glm::mat4* worldMatrix,
			glm::mat4* inverseWorldMatrix);
		void set(fwAABBox* modelAABB,
			glm::mat4* worldMatrix,
			glm::mat4* inverseWorldMatrix);
		void set(AABBoxTree* modelAABB,
			glm::mat4* worldMatrix,
			glm::mat4* inverseWorldMatrix);					// collider based on a gaAABB (split triangles by AABB)
		bool collision(const Collider& source,
			const glm::vec3& forward, 
			const glm::vec3& down,
			std::list<gaCollisionPoint>& collisions);							// run a collision test between 2 colliders
	};
}

