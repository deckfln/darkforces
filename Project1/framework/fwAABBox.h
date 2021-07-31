#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

class glBufferAttribute;
class fwSphere;
class fwCylinder;
class fwMesh;

#include "../flightRecorder/AABBox.h"

enum class fwAABBcollision {
	NONE = 0,
	TOP = 1,
	BOTTOM = 2,
	RIGHT = 4,
	LEFT = 8,
	FRONT = 16,
	BACK = 32
};

class fwAABBox
{
	fwAABBcollision m_collisionSide = fwAABBcollision::NONE;
	bool m_dirty = true;	// was updated => triggers a vertices update

	// draw the collision box
	fwMesh* m_mesh=nullptr;	

public:
	// create an impossible box, so 'extend' can work
	glm::vec3 m_p = glm::vec3(999999, 999999, 999999);
	glm::vec3 m_p1 = glm::vec3(-999999, -999999, -999999);

	fwAABBox();
	fwAABBox(float, float, float, float, float, float);
	fwAABBox(fwSphere& sphere);
	fwAABBox(const glm::vec3& p1, const glm::vec3& p2);
	fwAABBox(const fwAABBox& source, const glm::mat4& matrix);
	fwAABBox(const fwAABBox *source, const glm::mat4& matrix);
	fwAABBox(fwCylinder& cylinder);

	void set(float, float, float, float, float, float);	// build from points
	void set(std::vector<glm::vec3>& vertices);			// build from vertices
	void set(glBufferAttribute* attribute);				// build from an GL attribute
	void set(glm::vec3 const *pVertices, int nb);		// build from vertices pointers
	void set(const glm::vec3& p1, const glm::vec3& p2);	// build from 2 points

	fwAABBox operator+(const glm::vec3& v);
	fwAABBox& operator+=(const glm::vec3& v);

	fwAABBox& multiplyBy(float v);
	bool inside(const glm::vec3& position);
	bool inside(fwAABBox& box);
	bool isAbove(const fwAABBox& box);
	bool not_init(void);

	bool intersect(const fwAABBox& box);				// intersect with another AABB
	bool intersect(
		const glm::vec3& ray_orig, const glm::vec3& ray_dir, 
		glm::vec3 &point);								// intersect with a ray

	void extend(const fwAABBox& box);
	void extend(glm::vec3& vertice);
	float surface(void);
	float volume(void);
	void reset(void);
	const glm::vec3 center(void) const;
	float height(void);
	void translateFrom(const fwAABBox& source, glm::vec3& translation);
	void rotateFrom(const fwAABBox& source, const glm::vec3& rotation);
	void rotateFrom(const fwAABBox& source, const glm::quat& quaternion);
	void transform(const fwAABBox& source, glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);
	void transform(const fwAABBox& source, const glm::vec3& translation, const glm::quat& quaternion, const glm::vec3& scale);
	void transform(const glm::mat4& matrix);
	void apply(const fwAABBox& source, const glm::mat4& matrix);
	void apply(fwAABBox const *pSource, const glm::mat4& matrix);
	fwAABBox& copy(fwAABBox& source);
	fwAABBcollision collisionSide(void) { return m_collisionSide; };
	bool collision(fwAABBcollision side) { return (int)m_collisionSide & (int)side; };
	void collisionSide(fwAABBcollision side) { m_collisionSide = side; };
	glm::vec3 to(const fwAABBox& to);
	fwMesh *draw(void);
	bool updateMeshVertices(glm::vec3* vertices);
	bool verticalAlign(const glm::vec3& point);
	void recordState(flightRecorder::AABBox* record);	// record the state of the AABBox
	void loadState(flightRecorder::AABBox* record);	// reload the state of the AABBox
	~fwAABBox();
};