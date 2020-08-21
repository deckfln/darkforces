#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <list>

#include "gaCollisionPoint.h"

class fwAABBox;
class fwGeometry;
class fwCylinder;

namespace GameEngine
{
	class AABBoxTree;

	enum class ColliderType {
		NONE,
		AABB,
		ELIPSOID,
		GEOMETRY,
		AABB_TREE,
		CYLINDER
	};

	class Collider
	{
		ColliderType m_type=ColliderType::NONE;

		glm::mat4 const *m_worldMatrix = nullptr;
		glm::mat4 const *m_inverseWorldMatrix = nullptr;

		void* m_source = nullptr;

		static bool collision_fwAABB_geometry(const Collider& aabb, 
			const Collider& geometry, 
			const glm::vec3& forward,
			const glm::vec3& down,
			std::list<gaCollisionPoint>& collisions);		// do a fwAABB vs geometry collision check (test all geometry triangles)

		static bool collision_fwAABB_gaAABB(const Collider& fwAABB,
			const Collider& gaAABB,
			const glm::vec3& forward,
			const glm::vec3& down,
			std::list<gaCollisionPoint>& collisions);		// do a fwAABB vs gaAABB collision check (test only subset of triangles)

		static bool collision_cylinder_geometry(const Collider& cylinder,
			const Collider& geometry,
			const glm::vec3& forward,
			const glm::vec3& down,
			std::list<gaCollisionPoint>& collisions);		// do a fwAABB vs cylinder

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
			glm::mat4* inverseWorldMatrix);					// collider based on a a tree of AABB (split triangles by AABB)
		void set(fwCylinder* modelAABB,
			glm::mat4* worldMatrix,
			glm::mat4* inverseWorldMatrix);					// collider based on a cylinder
		bool collision(const Collider& source,
			const glm::vec3& forward, 
			const glm::vec3& down,
			std::list<gaCollisionPoint>& collisions);							// run a collision test between 2 colliders
	};
}

