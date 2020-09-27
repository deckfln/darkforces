#include "Collider.h"

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/norm.hpp>

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
	m_aabb(aabb),
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
	m_source = m_aabb = modelAABB;
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = inverseWorldMatrix;
}

/**
 * collider based on a gaAABB (split triangles by AABB)
 */
void Collider::set(AABBoxTree* modelAABB, glm::mat4* worldMatrix, glm::mat4* inverseWorldMatrix)
{
	m_type = ColliderType::AABB_TREE;
	m_source = m_aabb = modelAABB;
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
	m_aabb = nullptr;
	m_worldMatrix = worldMatrix;
	m_inverseWorldMatrix = inverseWorldMatrix;
}

/**
 * check 2 colliders after a successful worldAABB collision detection
 */
bool Collider::collision(const Collider& source, 
	const glm::vec3& forward,
	const glm::vec3& down,
	std::vector<gaCollisionPoint>& collisions)
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
			return collision_cylinder_aabb_tree(source, *this, collisions);
		}
		break;
	case ColliderType::GEOMETRY:
		switch (source.m_type) {
		case ColliderType::AABB:
			return collision_fwAABB_geometry(source, *this, forward, down, collisions);
		case ColliderType::AABB_TREE:
			return collision_fwAABB_geometry(source, *this, forward, down, collisions);
		case ColliderType::GEOMETRY:
			// convert to AABB vs Geometry
			return collision_fwAABB_geometry(*this, source, forward, down, collisions);
		case ColliderType::CYLINDER:
			return collision_cylinder_geometry(source, *this, collisions);
		}
		break;
	case ColliderType::CYLINDER:
		switch (source.m_type) {
		case ColliderType::AABB:
			return collision_cylinder_aabb(*this, source, collisions);
		case ColliderType::AABB_TREE:
			return collision_cylinder_aabb_tree(*this, source, collisions);
		case ColliderType::GEOMETRY:
			return collision_cylinder_geometry(*this, source, collisions);
		case ColliderType::CYLINDER:
			return collision_cylinder_cylinder(*this, source, collisions);
		}
		break;
	}

	return true;
}

/****************************************************************
 * test one sensor
 */

static bool testSensor(
	const glm::mat4& worldMatrix,
	const glm::vec3& p1,
	const glm::vec3& p2,
	const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
	glm::vec3& collision
)
{
	if (Framework::lineSegIntersectTri(p1, p2, a, b, c, collision)) {
		return true;
	}
/*
	static int debug = 0;
	double u, v, w, t;
	glm::vec3 point;
	glm::vec3 direction = p2 - p1;

	if (glm::intersectLineTriangle(p1, direction, a, b, c, point)) {
		// rebuild collision point (geometry space) to world space 
		double u, v, w;
		u = point.x;
		v = point.y;
		w = 1 - (u+v);

		collision.x = (u * a.x + v * b.x + w * c.x);
		collision.y = (u * a.y + v * b.y + w * c.y);
		collision.z = (u * a.z + v * b.z + w * c.z);

		collision = glm::vec3(worldMatrix * glm::vec4(collision, 1.0));

		return true;
	}
*/
/*
	if (Framework::IntersectLineTriangle(p1,
		p2,
		a, b, c,
		collision))
	{

		// check if point is on the segment
		float d = glm::dot(p2 - p1, collision - p1);
		if (d < 0) {
			return false;
		}
		if (d > glm::distance2(p2, p1)) {
			return false;
		}

		// rebuild collision point (geometry space) to world space 
		collision = glm::vec3(worldMatrix * glm::vec4(collision, 1.0));

		return true;
	};
*/
	return false;
}

/**************************************************************
 *
 */
bool Collider::warpThroughAABBTree(const Collider& aabbtree,
	const glm::vec3& position,
	const glm::vec3& old_position,
	std::vector<gaCollisionPoint>& collisions)
{
	// move the AABB from model space to worldSpace and then to geometry model space
	GameEngine::AABBoxTree* pAabbTree = static_cast<GameEngine::AABBoxTree*>(aabbtree.m_source);

	glm::mat4 mat = *aabbtree.m_inverseWorldMatrix;
	glm::vec3 position_gs = glm::vec3(mat * glm::vec4(position, 1.0));
	glm::vec3 old_position_gs = glm::vec3(mat * glm::vec4(old_position, 1.0));;
	glm::vec3 direction_gs = position_gs - old_position_gs;
	fwAABBox aabb_gs(position_gs, old_position_gs);

	glm::vec3 const* vertices_gs = pAabbTree->vertices();
	uint32_t nbVertices = pAabbTree->nbVertices();

	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	fwAABBox triangle;
	glm::vec3 collision;

	for (uint32_t i = 0; i < nbVertices; i += 3) {
		triangle.set(vertices_gs + i, 3);

		if (triangle.intersect(aabb_gs)) {
			if (testSensor(*aabbtree.m_worldMatrix,
				position_gs,
				old_position_gs,
				vertices_gs[i], vertices_gs[i + 1], vertices_gs[i + 2],
				collision)) 
			{
				// convert the collision point back to worldspace
				collision = glm::vec3(*aabbtree.m_worldMatrix * glm::vec4(collision, 1.0));
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::WARP, collision, vertices_gs + i));
			}
		}
	}

	return collisions.size() > 0;
}

/**
 * check if the entity moved so fast it went trough another one
 */
bool Collider::warpThrough(const Collider& source, 
	const glm::vec3& position, 
	const glm::vec3& old_position, 
	std::vector<gaCollisionPoint>& collisions)
{
	return warpThroughAABBTree(
		source,
		position, 
		old_position, 
		collisions);
}

/**
 * test the sensors
 */
static void testSensors(
	const glm::mat4 &worldMatrix,
	const glm::vec3 &center,
	const glm::vec3 &forward_geometry_space,
	const glm::vec3 &down,
	glm::vec3 const *triangle,
	std::vector<gaCollisionPoint>& collisions
)
{
	glm::vec3 collision;

	// check the forward sensor
	if (testSensor(worldMatrix, center, center + forward_geometry_space, triangle[0], triangle[1], triangle[2], collision))	{
		collisions.push_back(gaCollisionPoint(fwCollisionLocation::FRONT, collision, triangle));
	};

	// check the downward sensor
	if (testSensor(worldMatrix, center, center + down, triangle[0], triangle[1], triangle[2], collision)) {
		collisions.push_back(gaCollisionPoint(fwCollisionLocation::BOTTOM, collision, triangle));
	};
}

/**********************************************************
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
	std::vector<gaCollisionPoint>& collisions)
{
	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	fwAABBox triangle;
	glm::vec3 p2;
	glm::vec3 collision;

	for (uint32_t i = 0; i < nbVertices; i += 3) {
		triangle.set(vertices + i, 3);
		/*
		if (i == 384) {
			glm::vec3 p1 = center;
			glm::vec3 p2 = center + dwsensor;
			glm::vec3 a = vertices[i];
			glm::vec3 b = vertices[i + 1];
			glm::vec3 c = vertices[i + 2];
			float u, v, w, t;
			glm::vec3 point;

			bool b1 = Framework::IntersectSegmentTriangle(p1, p2, a, b, c, u, v, w, t);
			bool b2 = Framework::lineSegIntersectTri(p1, p2, a, b, c, point);
			bool b3 = glm::intersectLineTriangle(p1, dwsensor, a, b, c, point);
			bool b4 = Framework::IntersectLineTriangle(p1, p2, a, b, c, point);
			printf("\n");
		}
		*/
		if (triangle.intersect(aabb_geometry_space)) {
			p2 = center + forward_geometry_space;

			// check the forward sensor
			if (Framework::lineSegIntersectTri(center, p2, vertices[i], vertices[i + 1], vertices[i + 2], collision)) {
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::FRONT, collision, &vertices[i]));
			};

			p2 = center + dwsensor;
			// check the downward sensor
			if (Framework::lineSegIntersectTri(center, p2, vertices[i], vertices[i + 1], vertices[i + 2], collision)) {
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::BOTTOM, collision, &vertices[i]));
			};
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
	std::vector<gaCollisionPoint>& collisions)
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
		aabb.m_aabb,

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
	std::vector<gaCollisionPoint>& collisions)
{
	fwAABBox aabb_geometry_space;
	glm::vec3 center;
	glm::vec3 dwsensor;					// downward sensor
	glm::vec3 forward_geometry_space;
	GameEngine::AABBoxTree *pAABBtree = static_cast<GameEngine::AABBoxTree*>(gaAABB.m_source);

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
	if (!pAABBtree->find(aabb_geometry_space, hits)) {
		return false;
	}

	for (auto aabb : hits) {
		aabb_triangles(
			*gaAABB.m_worldMatrix,
			forward_geometry_space,
			dwsensor,
			center,
			aabb_geometry_space,
			pAABBtree->vertices(),
			pAABBtree->nbVertices(),
			collisions
		);
	}

	return collisions.size() != 0;
}

/************************************************
 *
 */
static void init_elipsoide(
	fwCylinder *cylinder,
	const glm::mat4& cylinder_worldMatrix,
	const glm::mat4& aabbtree_inverseWorldMatrix,
	glm::vec3& ellipsoid_space,
	glm::vec3& center_gs,
	glm::vec3& center_es,
	fwAABBox& cyl_aabb_gs
	)
{
	// extract the ellipsoid from the cylinder
	glm::vec3 ellipsoid(cylinder->radius(), cylinder->height() / 2.0f, cylinder->radius());

	// deform the model_space to make the ellipsoid  sphere
	ellipsoid_space = glm::vec3(1.0 / ellipsoid.x, 1.0 / ellipsoid.y, 1.0 / ellipsoid.z);

	// convert cylinder space (opengl world space) into the geometry space (model space)
	glm::mat4 mat = aabbtree_inverseWorldMatrix * cylinder_worldMatrix;
	glm::vec3 center_cs(0, cylinder->height()/2.0f, 0);
	center_gs = glm::vec3(mat * glm::vec4(center_cs, 1.0));

	cyl_aabb_gs = fwAABBox(*cylinder);
	cyl_aabb_gs.transform(mat);

	// and convert to ellipsoid space
	center_es = center_gs * ellipsoid_space;
}

/**
 * do a cylinder vs geometry collision check
 * test ALL triangles 
 */
bool Collider::collision_cylinder_geometry(
	const Collider& cylinder, 
	const Collider& geometry, 
	std::vector<gaCollisionPoint>& collisions)
{
	glm::vec3 ellipsoid_space;
	glm::vec3 center_gs;
	glm::vec3 center_es;
	fwAABBox cyl_aabb_gs;	// AABB enclosing the cylinder translated to the geometry space

	init_elipsoide(
		static_cast<fwCylinder*>(cylinder.m_source),
		*cylinder.m_worldMatrix,
		*geometry.m_inverseWorldMatrix,
		ellipsoid_space,
		center_gs,
		center_es,
		cyl_aabb_gs);

	// test each triangle vs the ellipsoid
	glm::vec3 v1_es, v2_es, v3_es;

	glm::vec3 const* vertices_gs = static_cast<fwGeometry*>(geometry.m_source)->vertices();
	uint32_t nbVertices = static_cast<fwGeometry*>(geometry.m_source)->nbvertices();
	glm::vec3 intersection_es;
	glm::vec3 intersection_gs;
	glm::vec3 intersection_ws;

	for (unsigned int i = 0; i < nbVertices; i += 3) {

		// convert each model space vertex to ellipsoid space
		v1_es = vertices_gs[i] * ellipsoid_space;
		v2_es = vertices_gs[i + 1] * ellipsoid_space;
		v3_es = vertices_gs[i + 2] * ellipsoid_space;

		if (Framework::intersectSphereTriangle(center_es, v1_es, v2_es, v3_es, intersection_es)) {
			// the intersection point is inside the triangle

			// convert the intersection from ellipsoid-space to cylinder-space
			intersection_gs = intersection_es / ellipsoid_space;

			// convert from cylinder-space => world-space
			intersection_ws = glm::vec3(*geometry.m_worldMatrix * glm::vec4(intersection_gs, 1.0));

			// inform if the collision point in world space(let the entity decide what to do with the collision)
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::COLLIDE, intersection_ws, &vertices_gs[i]));
		}
	}

	return collisions.size() != 0;
}

/**
 * do a fwAABB vs AABBTree (pointing to triangles)
 */
bool Collider::collision_cylinder_aabb_tree(const Collider& cylinder, 
	const Collider& aabb_tree, 
	std::vector<gaCollisionPoint>& collisions)
{
	glm::vec3 ellipsoid_space;
	glm::vec3 center_gs;
	glm::vec3 center_es;
	fwAABBox cyl_aabb_gs;

	init_elipsoide(
		static_cast<fwCylinder*>(cylinder.m_source),
		*cylinder.m_worldMatrix,
		*aabb_tree.m_inverseWorldMatrix,
		ellipsoid_space,
		center_gs,
		center_es,
		cyl_aabb_gs);

	GameEngine::AABBoxTree* pAabbTree = static_cast<GameEngine::AABBoxTree*>(aabb_tree.m_source);

	// find the smallest set of gaAABB intersecting with the fwAABB
	// and test only the included triangles
	// for each triangle, extract the AABB in geometry space and check collision with the source AABB in geometry space
	std::vector<GameEngine::AABBoxTree*> hits;
	if (!pAabbTree->find(cyl_aabb_gs, hits)) {
		return false;
	}

	glm::vec3 const* vertices_gs;
	uint32_t nbVertices;
	glm::vec3 v1_es, v2_es, v3_es;
	glm::vec3 intersection_es;
	glm::vec3 intersection_gs;
	glm::vec3 intersection_ws;
	fwAABBox triangle;

	for (auto aabb : hits) {
		vertices_gs = aabb->vertices();
		nbVertices = aabb->nbVertices();

		for (unsigned int i = 0; i < nbVertices; i += 3) {

			triangle.set(vertices_gs + i, 3);

			if (!triangle.intersect(cyl_aabb_gs)) {
				continue;
			}

			// convert each model space vertex to ellipsoid space
			v1_es = vertices_gs[i] * ellipsoid_space;
			v2_es = vertices_gs[i + 1] * ellipsoid_space;
			v3_es = vertices_gs[i + 2] * ellipsoid_space;

			/*
			if (i == 66 && nbVertices==594) {
				printf("Collider::collision_cylinder_aabb_tree\n");
			}
			*/
			if (Framework::intersectSphereTriangle(center_es, v1_es, v2_es, v3_es, intersection_es)) {
				// the intersection point is inside the triangle

				// convert the intersection from ellipsoid-space to cylinder-space
				intersection_gs = intersection_es / ellipsoid_space;

				// convert from cylinder-space => world-space
				intersection_ws = glm::vec3(*aabb_tree.m_worldMatrix * glm::vec4(intersection_gs, 1.0));

				// inform if the collision point in world space(let the entity decide what to do with the collision)
				collisions.push_back(gaCollisionPoint(fwCollisionLocation::COLLIDE, intersection_ws, &vertices_gs[i]));
			}
		}
	}

	return collisions.size() != 0;
}

/************************************************************************************************
 * cylinder vs AABB
 */
static const uint32_t _indexCube[] = {
	// back face
	0, // bottom-left
	1, // top-right
	2, // bottom-right         
	1, // top-right
	0, // bottom-left
	3, // top-left
	// front face
	4, // bottom-left
	5, // bottom-right
	6, // top-right
	6, // top-right
	7, // top-left
	4, // bottom-left
	// left face
	7, // top-right
	3, // top-left
	0, // bottom-left
	0, // bottom-left
	4, // bottom-right
	7, // top-right
	// right face
	6, // top-left
	2, // bottom-right
	1, // top-right         
	2, // bottom-right
	6, // top-left
	5, // bottom-left     
	// bottom face
	0, // top-right
	2, // top-left
	5, // bottom-left
	5, // bottom-left
	4, // bottom-right
	0, // top-right
	// top face
	3, // top-left
	6, // bottom-right
	1, // top-right     
	6, // bottom-right
	3, // top-left
	7 // bottom-left        
};
static glm::vec3 _vertexCube[8] = {
	{-1.0f, -1.0f, -1.0f},
	{ 1.0f,  1.0f, -1.0f},
	{ 1.0f, -1.0f, -1.0f},
	{-1.0f,  1.0f, -1.0f},
	{-1.0f, -1.0f,  1.0f},
	{ 1.0f, -1.0f,  1.0f},
	{ 1.0f,  1.0f,  1.0f},
	{-1.0f,  1.0f,  1.0f}
};
static glm::vec3 _triangles[36][3];

static void convertVertex(const uint32_t i, const float x, const float y, const float z, const glm::mat4& mat)
{
	_vertexCube[i] = mat * glm::vec4(x, y, z, 1.0);
}

bool Collider::collision_cylinder_aabb(const Collider& cylinder, 
	const Collider& aabb, 
	std::vector<gaCollisionPoint>& collisions)
{
	glm::vec3 ellipsoid_space;
	glm::vec3 center_gs;
	glm::vec3 center_es;
	fwAABBox cyl_aabb_gs;

	/*
	init_elipsoide(
		static_cast<fwCylinder*>(cylinder.m_source),
		*cylinder.m_worldMatrix,
		*aabb.m_inverseWorldMatrix,
		ellipsoid_space,
		center_gs,
		center_es,
		cyl_aabb_gs);
	*/
	// extract the ellipsoid from the cylinder
	fwCylinder* pCylinder = static_cast<fwCylinder*>(cylinder.m_source);
	glm::vec3 ellipsoid(pCylinder->radius(), pCylinder->height() / 2.0f, pCylinder->radius());

	// deform the model_space to make the ellipsoid  sphere
	ellipsoid_space = glm::vec3(1.0 / ellipsoid.x, 1.0 / ellipsoid.y, 1.0 / ellipsoid.z);

	// convert cylinder space (opengl world space) into the geometry space (model space)
	glm::mat4 mat = *cylinder.m_worldMatrix;
	glm::vec3 center_cs(0, pCylinder->height() / 2.0f, 0);
	center_gs = glm::vec3(mat * glm::vec4(center_cs, 1.0));

	//cyl_aabb_gs = fwAABBox(*pCylinder);
	//cyl_aabb_gs.transform(mat);

	// and convert to ellipsoid space
	center_es = center_cs * ellipsoid_space;

	glm::vec3 v1_es, v2_es, v3_es;
	glm::vec3 intersection_es;
	glm::vec3 intersection_gs;
	glm::vec3 intersection_ws;

	fwAABBox* pAabb = static_cast<fwAABBox*>(aabb.m_source);

	// convert AABB space into the cylinder space
	mat = *cylinder.m_inverseWorldMatrix * *aabb.m_worldMatrix;
	convertVertex(0, pAabb->m_p.x, pAabb->m_p.y, pAabb->m_p.z, mat);
	convertVertex(1, pAabb->m_p1.x, pAabb->m_p1.y, pAabb->m_p.z, mat);
	convertVertex(2, pAabb->m_p1.x, pAabb->m_p.y, pAabb->m_p.z, mat);
	convertVertex(3, pAabb->m_p.x, pAabb->m_p1.y, pAabb->m_p.z, mat);
	convertVertex(4, pAabb->m_p.x, pAabb->m_p.y, pAabb->m_p1.z, mat);
	convertVertex(5, pAabb->m_p1.x, pAabb->m_p.y, pAabb->m_p1.z, mat);
	convertVertex(6, pAabb->m_p1.x, pAabb->m_p1.y, pAabb->m_p1.z, mat);
	convertVertex(7, pAabb->m_p.x, pAabb->m_p1.y, pAabb->m_p1.z, mat);

	for (unsigned int i = 0; i < 36; i += 3) {
		// extract triangle
		_triangles[i][0] = _vertexCube[_indexCube[i]];
		_triangles[i][1] = _vertexCube[_indexCube[i + 1]];
		_triangles[i][2] = _vertexCube[_indexCube[i + 2]];

		// convert each model space vertex to ellipsoid space
		v1_es = _triangles[i][0] * ellipsoid_space;
		v2_es = _triangles[i][1] * ellipsoid_space;
		v3_es = _triangles[i][2] * ellipsoid_space;

		v1_es.y--;
		v2_es.y--;
		v3_es.y--;

		if (Framework::intersectSphereTriangle(glm::vec3(0), v1_es, v2_es, v3_es, intersection_es)) {
			// the intersection point is inside the triangle

			// convert the intersection from ellipsoid-space to cylinder-space
			intersection_gs = intersection_es / ellipsoid_space;

			// convert from cylinder-space => world-space
			//intersection_ws = glm::vec3(*aabb.m_worldMatrix * glm::vec4(intersection_gs, 1.0));

			// inform if the collision point in world space(let the entity decide what to do with the collision)
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::COLLIDE, intersection_gs, _triangles[i]));
		}
	}

	return collisions.size() != 0;
}

/******************************************************************************
 * cylinder vs ellipsoid
 */
static glm::vec3 _vertexSphereBase[6] = {
	{-1.0f,  0.0f,  0.0f},
	{ 0.0f,  0.0f,  1.0f},
	{ 1.0f,  0.0f,  0.0f},
	{ 0.0f,  0.0f, -1.0f},
	{ 0.0f,  1.0f,  0.0f},
	{ 0.0f,  1.0f,  0.0f}
};
static glm::vec3 _vertexSphere[6];
static const uint32_t _indexSphere[] = {
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4,
	0, 5, 1,
	1, 5, 2,
	2, 5, 3,
	3, 5, 0
};
static void convertVertexSphere(const uint32_t i, const glm::mat4& mat)
{
	_vertexSphere[i] = mat * glm::vec4(_vertexSphereBase[i], 1.0);
}

bool Collider::collision_cylinder_cylinder(const Collider& cylinder1,
	const Collider& cylinder2,
	std::vector<gaCollisionPoint>& collisions)
{
	glm::vec3 ellipsoid1_space;
	glm::vec3 center_gs;
	glm::vec3 center_es;
	fwAABBox cyl_aabb_gs;

	// extract the ellipsoid from the cylinder
	fwCylinder* pCylinder = static_cast<fwCylinder*>(cylinder1.m_source);
	glm::vec3 ellipsoid1(pCylinder->radius(), pCylinder->height() / 2.0f, pCylinder->radius());

	// deform the model_space to make the ellipsoid  sphere
	ellipsoid1_space = glm::vec3(1.0 / ellipsoid1.x, 1.0 / ellipsoid1.y, 1.0 / ellipsoid1.z);

	// convert cylinder space (opengl world space) into the geometry space (model space)
	glm::mat4 mat = *cylinder1.m_worldMatrix;
	glm::vec3 center_cs(0, pCylinder->height() / 2.0f, 0);
	center_gs = glm::vec3(mat * glm::vec4(center_cs, 1.0));

	//cyl_aabb_gs = fwAABBox(*pCylinder);
	//cyl_aabb_gs.transform(mat);

	// and convert to ellipsoid space
	center_es = center_cs * ellipsoid1_space;

	glm::vec3 v1_es, v2_es, v3_es;
	glm::vec3 intersection_es;
	glm::vec3 intersection_gs;
	glm::vec3 intersection_ws;

	fwCylinder* pCylinder2 = static_cast<fwCylinder*>(cylinder2.m_source);
	glm::mat4 ellipsoid2_space(1.0f);
	ellipsoid2_space[0][0] = pCylinder2->radius();
	ellipsoid2_space[1][1] = pCylinder2->height() / 2.0f;
	ellipsoid2_space[2][2] = pCylinder2->radius();

	// convert cylinder2 space into cylinder1 space
	mat = *cylinder1.m_inverseWorldMatrix * *cylinder2.m_worldMatrix * ellipsoid2_space;
	convertVertexSphere(0, mat);
	convertVertexSphere(1, mat);
	convertVertexSphere(2, mat);
	convertVertexSphere(3, mat);
	convertVertexSphere(4, mat);
	convertVertexSphere(5, mat);

	for (unsigned int i = 0; i < 24; i += 3) {
		// extract triangle
		_triangles[i][0] = _vertexSphere[_indexSphere[i]];
		_triangles[i][1] = _vertexSphere[_indexSphere[i + 1]];
		_triangles[i][2] = _vertexSphere[_indexSphere[i + 2]];

		// convert each model space vertex to ellipsoid space
		v1_es = _triangles[i][0] * ellipsoid1_space;
		v2_es = _triangles[i][1] * ellipsoid1_space;
		v3_es = _triangles[i][2] * ellipsoid1_space;

		v1_es.y--;
		v2_es.y--;
		v3_es.y--;

		if (Framework::intersectSphereTriangle(glm::vec3(0), v1_es, v2_es, v3_es, intersection_es)) {
			// the intersection point is inside the triangle

			// convert the intersection from ellipsoid-space to cylinder-space
			intersection_gs = intersection_es / ellipsoid1_space;

			// convert from cylinder-space => world-space
			//intersection_ws = glm::vec3(*aabb.m_worldMatrix * glm::vec4(intersection_gs, 1.0));

			// inform if the collision point in world space(let the entity decide what to do with the collision)
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::COLLIDE, intersection_gs, _triangles[i]));
		}
	}

	return collisions.size() != 0;
}

/******************************************************************************
 * run a collision with a segment
 */
bool Collider::collision(const glm::vec3& start, const glm::vec3& end)
{
	const glm::mat4& inverseWorldMatrix = *m_inverseWorldMatrix;
	glm::vec3 p1 = glm::vec3(inverseWorldMatrix * glm::vec4(start, 1.0));
	glm::vec3 p2 = glm::vec3(inverseWorldMatrix * glm::vec4(end, 1.0));

	GameEngine::AABBoxTree* pAabbTree = static_cast<GameEngine::AABBoxTree*>(m_source);

	glm::vec3 const* vertices_gs = pAabbTree->vertices();
	uint32_t nbVertices = pAabbTree->nbVertices();
	glm::vec3 collision;

	for (unsigned int i = 0; i < nbVertices; i += 3) {
		if (testSensor(inverseWorldMatrix,
			p1, p2,
			vertices_gs[i], vertices_gs[i + 1], vertices_gs[i + 2],
			collision)) {
			return true;
		}
	}

	return false;
}
