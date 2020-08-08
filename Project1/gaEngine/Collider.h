#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class fwAABBox;
class fwGeometry;

namespace GameEngine
{
	enum class ColliderType {
		NONE,
		AABB,
		ELIPSOID,
		GEOMETRY
	};

	class Collider
	{
		ColliderType m_type=ColliderType::NONE;

		const glm::mat4 *m_worldMatrix = nullptr;
		const glm::mat4 *m_inverseWorldMatrix = nullptr;

		fwAABBox *m_aabb=nullptr;
		fwGeometry *m_geometry=nullptr;

		bool collisionAABBgeometry(const Collider& aabb, 
			const Collider& geometry, 
			const glm::vec3& direction, 
			glm::vec3& collision);							// do a AABB vs geometry collision check. Return the first collision

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
		bool collision(const Collider& source,
			const glm::vec3& direction, 
			glm::vec3& collision);							// run a collision test between 2 colliders
	};
}

