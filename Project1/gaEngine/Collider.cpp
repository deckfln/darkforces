#include "Collider.h"

#include <glm/gtx/intersect.hpp>

#include "../framework/fwAABBox.h"
#include "../framework/fwGeometry.h"
#include "../framework/fwCollision.h"
#include "../framework/math/fwCylinder.h"

#include "AABBoxTree.h"

using namespace GameEngine;

/**
 * Initialize a AABBox as collider
 */
Collider::Collider(fwAABBox* aabb, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix):
	m_type(ColliderType::AABB),
	m_source(aabb),
	m_worldMatrix(worldMatrix),
	m_inverseWorldMatrix(inverseWorldMatrix)
{
}

/**
 * Initialize a geometry as collider
 */
Collider::Collider(fwGeometry* geometry, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix) :
	m_type(ColliderType::GEOMETRY),
	m_source(geometry),
	m_worldMatrix(worldMatrix),
	m_inverseWorldMatrix(inverseWorldMatrix)
{
}

/**
 * force the objects after creation
 */
void Collider::set(fwGeometry* geometry, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix)
{
	m_type = ColliderType::GEOMETRY;
	m_source = geometry;
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = inverseWorldMatrix;
}

void Collider::set(fwAABBox* modelAABB, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix)
{
	m_type = ColliderType::AABB;
	m_source = modelAABB;
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = inverseWorldMatrix;
}

/**
 * collider based on a gaAABB (split triangles by AABB)
 */
void Collider::set(AABBoxTree* modelAABB, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix)
{
	m_type = ColliderType::AABB_TREE;
	m_source = modelAABB;
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = inverseWorldMatrix;
}

/**
 * collider based on a cylinder
 */
void Collider::set(fwCylinder* cylinder, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix)
{
	m_type = ColliderType::CYLINDER;
	m_source = cylinder;
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
	switch (m_type) {
	case ColliderType::AABB:
		switch (source.m_type) {
		case ColliderType::AABB:
			return true;	// the worldAABB already collide, no need for further test
		case ColliderType::AABB_TREE:
			return collision_fwAABB_gaAABB(*this, source, forward, down, collisions);
		case ColliderType::GEOMETRY:
			return collision_fwAABB_geometry(*this, source, forward, down, collisions);
		case ColliderType::CYLINDER:
			break;
		}
		break;
	case ColliderType::AABB_TREE:
		switch (source.m_type) {
		case ColliderType::AABB:
			return collision_fwAABB_gaAABB(source, *this, forward, down, collisions);
		case ColliderType::AABB_TREE:
			printf("Collider::collision GA_AABB vs GA_AABB not implemented");
			return true;
		case ColliderType::GEOMETRY:
			printf("Collider::collision GA_AABB vs Geometry not implemented");
			return true;
		case ColliderType::CYLINDER:
			return true;
		}
		break;
	case ColliderType::GEOMETRY:
		switch (source.m_type) {
		case ColliderType::AABB:
			return collision_fwAABB_geometry(source, *this, forward, down, collisions);
		case ColliderType::AABB_TREE:
			printf("Collider::collision Geometry vs GA_AABB not implemented");
			return true;
		case ColliderType::GEOMETRY:
			printf("Collider::collision Geometry vs Geometry not implemented");
			return true;
		case ColliderType::CYLINDER:
			return collision_cylinder_geometry(source, *this, forward, down, collisions);
		}
		break;
	case ColliderType::CYLINDER:
		switch (source.m_type) {
		case ColliderType::AABB:
			return true;
		case ColliderType::AABB_TREE:
			return true;
		case ColliderType::GEOMETRY:
			return collision_cylinder_geometry(*this, source, forward, down, collisions);
		case ColliderType::CYLINDER:
			return true;
		}
		break;
	}

	return true;
}

/**
 * test one sensor
 */
static bool testSensor(
	const glm::mat4& worldMatrix,
	const glm::vec3& center,
	const glm::vec3& direction,
	const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
	glm::vec3& collision
)
{
	float u, v, w;

	// check the forward sensor
	if (glm::intersectLineTriangle(center,
		direction,
		a, b, c,
		collision))
	{
		u = collision.x;
		v = collision.y;
		w = 1 - (u + v);

		collision = (u * a + v * b + w * c);

		// rebuild collision point (geometry space) to world space 
		collision = glm::vec3(worldMatrix * glm::vec4(collision, 1.0));

		return true;
	};

	return false;
}

/**
 * test the sensors
 */
static void testSensors(
	const glm::mat4 &worldMatrix,
	const glm::vec3 &center,
	const glm::vec3 &forward_geometry_space,
	const glm::vec3 &down,
	const glm::vec3 &a, const glm::vec3& b, const glm::vec3& c,
	std::list<gaCollisionPoint>& collisions
)
{
	glm::vec3 collision;

	// check the forward sensor
	if (testSensor(worldMatrix, center, forward_geometry_space, a,b,c, collision))	{
		collisions.push_back(gaCollisionPoint(fwCollisionLocation::FRONT, collision, nullptr));
	};

	// check the downward sensor
	if (testSensor(worldMatrix, center, down, a, b, c, collision)) {
		collisions.push_back(gaCollisionPoint(fwCollisionLocation::BOTTOM, collision, nullptr));
	};
}

/**
 * init aabb vs triangles
 */
static void init_aabb_triangles(
	const glm::mat4& aabb_worldMatrix,
	const glm::mat4& geometry_inverseWorldMatrix,
	const glm::vec3& forward,
	const glm::vec3& down,
	fwAABBox const* aabb_space,

	glm::vec3& forward_geometry_space,
	glm::vec3& dwsensor,
	glm::vec3& center,
	fwAABBox& aabb_geometry_space
	)
{
	// move the AABB from model space to worldSpace and then to geometry model space
	glm::mat4 mat = geometry_inverseWorldMatrix * aabb_worldMatrix;
	aabb_geometry_space.apply(aabb_space, mat);

	// remove the translation part, this is a direction vector
	glm::mat4 rotation_scale = geometry_inverseWorldMatrix;
	rotation_scale[3][0] = rotation_scale[3][1] = rotation_scale[3][2] = 0;
	forward_geometry_space = glm::vec3(rotation_scale * glm::vec4(forward, 1.0));

	// half of the length of the source AABB
	float half_height = (aabb_space->m_p1.y - aabb_space->m_p.y) / 2.0f;
	center = glm::vec3(mat * glm::vec4(0, 0, 0, 1));
	dwsensor = down * half_height;					// downward sensor
}

/**
 * test an AABB (in model space) vs triangles in a geometry
 */
static void aabb_triangles(
	const glm::mat4& worldMatrix,
	const glm::vec3& forward_geometry_space,
	const glm::vec3& dwsensor,
	const glm::vec3& center,
	const fwAABBox& aabb_geometry_space,
	glm::vec3 const* vertices,
	uint32_t nbVertices,
	std::list<gaCollisionPoint>& collisions)
{
	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	fwAABBox triangle;

	for (uint32_t i = 0; i < nbVertices; i += 3) {
		triangle.set(vertices + i, 3);

		if (triangle.intersect(aabb_geometry_space)) {
			testSensors(worldMatrix,
				center,
				forward_geometry_space,
				dwsensor,
				vertices[i], vertices[i + 1], vertices[i + 2],
				collisions);
		}
	}
}

/**
 * doing AABB vs geometry collision
 * test all triangles of the geometry
 */
bool Collider::collision_fwAABB_geometry(const Collider& aabb, 
	const Collider& geometry, 
	const glm::vec3& forward, // in world space
	const glm::vec3& down,
	std::list<gaCollisionPoint>& collisions)
{
	glm::vec3 const* vertices = static_cast<fwGeometry*>(geometry.m_source)->vertices();
	uint32_t nbVertices = static_cast<fwGeometry*>(geometry.m_source)->nbvertices();

	fwAABBox aabb_geometry_space;
	glm::vec3 center;
	glm::vec3 dwsensor;					// downward sensor
	glm::vec3 forward_geometry_space;

	init_aabb_triangles(
		*aabb.m_worldMatrix,
		*geometry.m_inverseWorldMatrix,
		forward,
		down,
		static_cast<fwAABBox*>(aabb.m_source),

		forward_geometry_space,
		dwsensor,
		center,
		aabb_geometry_space
	);

	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	aabb_triangles(
		*geometry.m_worldMatrix,
		forward_geometry_space,
		dwsensor,
		center,
		aabb_geometry_space,
		vertices,
		nbVertices,
		collisions
	);

	return collisions.size() != 0;
}

/**
 * do a fwAABB vs gaAABB collision check
 * only test triangles included in the smallest gaAABB hierarchy
 */
bool Collider::collision_fwAABB_gaAABB(const Collider& fwAABB, 
	const Collider& gaAABB, 
	const glm::vec3& forward, 
	const glm::vec3& down, 
	std::list<gaCollisionPoint>& collisions)
{
	fwAABBox aabb_geometry_space;
	glm::vec3 center;
	glm::vec3 dwsensor;					// downward sensor
	glm::vec3 forward_geometry_space;

	init_aabb_triangles(
		*fwAABB.m_worldMatrix,
		*gaAABB.m_inverseWorldMatrix,
		forward,
		down,
		static_cast<fwAABBox*>(fwAABB.m_source),

		forward_geometry_space,
		dwsensor,
		center,
		aabb_geometry_space
	);

	// find the smallest set of gaAABB intersecting with the fwAABB
	// and test only the included triangles
	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	std::vector<GameEngine::AABBoxTree*> hits;
	if (!static_cast<GameEngine::AABBoxTree*>(gaAABB.m_source)->find(aabb_geometry_space, hits)) {
		return false;
	}

	for (auto aabb : hits) {
		aabb_triangles(
			*gaAABB.m_worldMatrix,
			forward_geometry_space,
			dwsensor,
			center,
			aabb_geometry_space,
			aabb->vertices(),
			aabb->nbVertices(),
			collisions
		);
	}

	return collisions.size() != 0;
}

/**
 * do a cylinder vs geometry collision check
 * test ALL triangles 
 */
bool Collider::collision_cylinder_geometry(
	const Collider& cylinder, 
	const Collider& geometry, 
	const glm::vec3& forward, 
	const glm::vec3& down, 
	std::list<gaCollisionPoint>& collisions)
{
	fwCylinder* cyl = static_cast<fwCylinder*>(cylinder.m_source);
	fwAABBox aabb(*cyl);	// convert to AABB for fast test

	// extract the ellipsoid from the cylinder
	glm::vec3 ellipsoid(cyl->height() / 2.0f, cyl->radius(), cyl->radius());

	// deform the model_space to make the ellipsoid  sphere
	glm::vec3 ellipsoid_space(1.0 / ellipsoid.x, 1.0 / ellipsoid.y, 1.0 / ellipsoid.z);

	// convert cylinder space (opengl world space) into the geometry space (model space)
	glm::mat4 mat = *geometry.m_inverseWorldMatrix * *cylinder.m_worldMatrix;
	glm::vec3 center_cs(0, cyl->height(), 0);
	glm::vec3 center_gs = glm::vec3(mat * glm::vec4(center_cs, 1.0));

	// remove the translation part, this is a direction vector
	glm::mat4 rotation_scale = *geometry.m_inverseWorldMatrix;
	rotation_scale[3][0] = rotation_scale[3][1] = rotation_scale[3][2] = 0;
	glm::vec3 forward_gs = glm::vec3(rotation_scale * glm::vec4(forward, 1.0));

	// and convert to ellipsoid space
	glm::vec3 center_es = center_gs * ellipsoid_space;

	// test each triangle vs the ellipsoid
	glm::vec3 v1_es, v2_es, v3_es;
	glm::vec3 intersection_es;

	glm::vec3 const* vertices_gs = static_cast<fwGeometry*>(geometry.m_source)->vertices();
	uint32_t nbVertices = static_cast<fwGeometry*>(geometry.m_source)->nbvertices();

	for (unsigned int i = 0; i < nbVertices; i += 3) {

		// convert each model space vertex to ellipsoid space
		v1_es = vertices_gs[i] * ellipsoid_space;
		v2_es = vertices_gs[i + 1] * ellipsoid_space;
		v3_es = vertices_gs[i + 2] * ellipsoid_space;

		if (Framework::intersectSphereTriangle(center_es, v1_es, v2_es, v3_es, intersection_es)) {
			// the intersection point is inside the triangle

			// convert the intersection point back to model space
			//intersection_gs = intersection_es / ellipsoid_space;

			// convert from (model space) intersection to (level space) intersection
			//intersection_ws = glm::vec3(*geometry.m_worldMatrix() * glm::vec4(intersection_ms, 1.0));

			testSensors(*geometry.m_worldMatrix,
				center_gs,
				forward_gs,
				down,
				vertices_gs[i], vertices_gs[i + 1], vertices_gs[i + 2],
				collisions);
		}
	}

	return collisions.size() != 0;
}
