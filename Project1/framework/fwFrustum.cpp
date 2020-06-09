#include "fwFrustum.h"

fwFrustum::fwFrustum()
{
}

fwFrustum &fwFrustum::setFromMatrix(glm::mat4 matrix)
{
	float me0 = matrix[0][0], me1 = matrix[0][1], me2 = matrix[0][2], me3 = matrix[0][3];
	float me4 = matrix[1][0], me5 = matrix[1][1], me6 = matrix[1][2], me7 = matrix[1][3];
	float me8 = matrix[2][0], me9 = matrix[2][1], me10 = matrix[2][2], me11 = matrix[2][3];
	float me12 = matrix[3][0], me13 = matrix[3][1], me14 = matrix[3][2], me15 = matrix[3][3];

	m_planes[0].components(me3 - me0, me7 - me4, me11 - me8, me15 - me12).normalize();
	m_planes[1].components(me3 + me0, me7 + me4, me11 + me8, me15 + me12).normalize();
	m_planes[2].components(me3 + me1, me7 + me5, me11 + me9, me15 + me13).normalize();
	m_planes[3].components(me3 - me1, me7 - me5, me11 - me9, me15 - me13).normalize();
	m_planes[4].components(me3 - me2, me7 - me6, me11 - me10, me15 - me14).normalize();
	m_planes[5].components(me3 + me2, me7 + me6, me11 + me10, me15 + me14).normalize();

	return *this;
}

bool fwFrustum::intersectsObject(fwMesh *object)
{
	fwGeometry *geometry = object->get_geometry();
	fwSphere sphere;

	if (geometry->boundingsphere() == nullptr) {
		geometry->computeBoundingsphere();
	}

	sphere.applyMatrix4From(object->worldMatrix(), geometry->boundingsphere());

	return intersectsSphere(sphere, geometry->boundingsphere());
}

bool fwFrustum::intersectsSphere(fwSphere &sphere, fwSphere *source)
{
	glm::vec3 center = sphere.center();
	float negRadius = -sphere.radius();
	float distance = -1;

	if (source->cache() >= 0) {
		distance = m_planes[source->cache()].distanceToPoint(center);
		if (distance < negRadius) {
			return false;
		}
	}

	for (int p = 0; p < 6; ++p) {
		if (p == source->cache()) {
			continue;
		}

		distance = m_planes[p].distanceToPoint(center);

		if (distance < negRadius) {
			source->cache(p);
			return false;
		}
	}

	source->cache(-1);
	return true;
}

fwFrustum::~fwFrustum()
{
}