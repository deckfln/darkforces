#include "fwCollision.h"

#include <glm/glm.hpp>
#include <algorithm>

fwCollision::fwCollision()
{
}

fwCollision::~fwCollision(void)
{
}

// Christer_Ericson-Real-Time_Collision_Detectio
// Given segment pq and triangle abc, returns whether segment intersects
// triangle and if so, also returns the barycentric coordinates (u,v,w)
// of the intersection point
bool Framework::IntersectSegmentTriangle(const glm::vec3& p,
	const glm::vec3& q, 
	const glm::vec3& a,
	const glm::vec3& b, 
	const glm::vec3& c,
	float &u, 
	float &v, 
	float &w, 
	float &t)
{
	glm::vec3 ab=b-a;
	glm::vec3 ac=c-a;
	glm::vec3 qp=p-q;
	
	// Compute triangle normal. Can be precalculated or cached if
	// intersecting multiple segments against the same triangle
	glm::vec3 n = glm::cross(ab, ac);
	
	// Compute denominator d. If d <= 0, segment is parallel to or points
	// away from triangle, so exit early
	float d = glm::dot(qp, n);
	if (d <= 0.0f) 
		return false;
	
	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
	// dividing by d until intersection has been found to pierce triangle
	glm::vec3 ap = p - a; 
	t = glm::dot(ap, n); 
	if (t < 0.0f) 
		return false; 
	
	if (t > d) 
		return false;
	
	// For segment; exclude this code line for a ray test
	// Compute barycentric coordinate components and test if within bounds
	glm::vec3 e = glm::cross(qp, ap);
	v = glm::dot(ac, e);
	if (v < 0.0f || v>d)
		return false;
	
	w = -glm::dot(ab, e);
	if (w < 0.0f || v+w>d)
		return false;
	
	// Segment/ray intersects triangle. Perform delayed division and
	// compute the last barycentric coordinate component
	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f-v-w;
	return true;
}

// https://www.thetopsites.net/article/53962225.shtml
const float EPSILON = 0.001f;

bool Framework::lineSegIntersectTri(
	const glm::vec3 &p,
	const glm::vec3 &q,
	const glm::vec3& a,
	const glm::vec3& b,
	const glm::vec3& c,
	fwCollision::Test test,
	glm::vec3  &point
) {
	glm::vec3 e0 = b - a;
	glm::vec3 e1 = c - a;

	glm::vec3 dir = q - p;
	glm::vec3 dir_norm = glm::normalize(dir);

	glm::vec3 h = glm::cross(dir_norm, e1);
	const float d = glm::dot(e0, h);

	if (d > -EPSILON && d < EPSILON) {
		return false;
	}

	glm::vec3 s = p - a;
	const float f = 1.0f / d;
	const float u = f * glm::dot(s, h);

	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	glm::vec3 q1 = glm::cross(s, e0);
	const float v = f * glm::dot(dir_norm, q1);

	if (v < 0.0f || u + v > 1.0f) {
		return false;
	}

	const float t = f * glm::dot(e1, q1);

	if (test == fwCollision::Test::WITH_BORDERS) {
		if (t >= 0 && t < sqrtf(glm::dot(dir, dir))) { // segment intersection
			point = p + dir_norm * t;

			return true;
		}
	}
	else {
		float t1 = sqrtf(glm::dot(dir, dir)) - EPSILON;
		float a = t - EPSILON;
		if (a >= 0 && t <= t1) { // segment intersection
			point = p + dir_norm * t;

			return true;
		}
	}

	return false;
}

/**
 * Sphere/satNavTriangle intersection (other version)
 */
bool Framework::intersectSphereTriangle(
	const glm::vec3& center_es, 
	const glm::vec3& a,	const glm::vec3& b, const glm::vec3& c, 
	glm::vec3& p)
{
	// plane equation
	glm::vec3 normal;
	glm::vec4 plane;
	float signedDistance;

	// triangle equation
	glm::vec3 tr0;
	glm::vec3 tr1;
	float tarea, tarea1, tarea2, tarea3;

	// first, test the intersection with the triangle plane
	normal = glm::normalize(glm::cross(b - a, c - a));
	plane = glm::vec4(normal.x, normal.y, normal.z, -(normal.x * a.x + normal.y * a.y + normal.z * a.z));
	signedDistance = glm::dot(center_es, normal) + plane.w;

	if (signedDistance >= -1.0f - EPSILON && signedDistance <= 1.0f + EPSILON) {
		// if the plane is passing trough the sphere (the ellipsoid deformed to look like a sphere)
		// get the collision point of the sphere on the plane

		p = center_es - normal * glm::abs(signedDistance);

		// test if the collision origin in INSIDE the triangle

		// area of the GL triangle
		tr0 = a - b;
		tr1 = c - b;
		tarea = glm::length(glm::cross(tr0, tr1));

		// area of the triangles using the intersection point as vertex
		tr0 = a - p;
		tr1 = b - p;
		tarea1 = glm::length(glm::cross(tr0, tr1));

		tr0 = b - p;
		tr1 = c - p;
		tarea2 = glm::length(glm::cross(tr0, tr1));

		tr0 = c - p;
		tr1 = a - p;
		tarea3 = glm::length(glm::cross(tr0, tr1));

		// if the sum of the 3 new triangles is equal to the opengl triangle
		if (abs(tarea1 + tarea2 + tarea3 - tarea) < 0.01) {
			return true;
		}

		// the point might be away, so test the edges
		if (Framework::IntersectionSphereLine(center_es, 1.0f, a, b, p)) {
			return true;
		}
		if (Framework::IntersectionSphereLine(center_es, 1.0f, b, c, p)) {
			return true;
		}
		if (Framework::IntersectionSphereLine(center_es, 1.0f, c, a, p)) {
			return true;
		}
	}

	return false;
}

/**
 * Sphere/Triangle intersection with sphere as the origin
 */
bool Framework::intersectSphereOriginTriangle(
	const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
	glm::vec3& p)
{
	// plane equation
	glm::vec3 normal;
	glm::vec4 plane;
	float signedDistance;

	// triangle equation
	glm::vec3 tr0;
	glm::vec3 tr1;
	float tarea, tarea1, tarea2, tarea3;

	// first, test the intersection with the triangle plane
	normal = glm::normalize(glm::cross(b - a, c - a));
	plane = glm::vec4(normal.x, normal.y, normal.z, -(normal.x * a.x + normal.y * a.y + normal.z * a.z));
	signedDistance = plane.w;

	if (signedDistance >= -1.0f - EPSILON && signedDistance <= 1.0f + EPSILON) {
		// if the plane is passing trough the sphere (the ellipsoid deformed to look like a sphere)
		// get the collision point of the sphere on the plane

		p = -normal * signedDistance;

		// test if the collision origin in INSIDE the triangle

		// area of the GL triangle
		tr0 = a - b;
		tr1 = c - b;
		tarea = glm::length(glm::cross(tr0, tr1));

		// area of the triangles using the intersection point as vertex
		tr0 = a - p;
		tr1 = b - p;
		tarea1 = glm::length(glm::cross(tr0, tr1));

		tr0 = b - p;
		tr1 = c - p;
		tarea2 = glm::length(glm::cross(tr0, tr1));

		tr0 = c - p;
		tr1 = a - p;
		tarea3 = glm::length(glm::cross(tr0, tr1));

		// if the sum of the 3 new triangles is equal to the opengl triangle
		if (abs(tarea1 + tarea2 + tarea3 - tarea) < 0.01) {
			return true;
		}

		// the point might be away, so test the edges
		if (Framework::IntersectionSphereLine(glm::vec3(0), 1.0f, a, b, p)) {
			return true;
		}
		if (Framework::IntersectionSphereLine(glm::vec3(0), 1.0f, b, c, p)) {
			return true;
		}
		if (Framework::IntersectionSphereLine(glm::vec3(0), 1.0f, c, a, p)) {
			return true;
		}
	}

	return false;
}

/********************************************************
 * https://gamedev.stackexchange.com/questions/65723/finding-z-given-x-y-coordinates-on-terrain
 */
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

static bool same_sign(const float a, const float b) {
	return sgn(a) == sgn(b);
}

static float scalar_product(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
	return glm::dot(glm::cross(a, b), c);
}

bool Framework::IntersectLineTriangle(
	const glm::vec3& p, const glm::vec3& q, 
	const glm::vec3& a, const glm::vec3& b, const glm::vec3& c,
	glm::vec3& collision)
{
	const glm::vec3 pq = q - p;
	const glm::vec3 pa = a - p;
	const glm::vec3 pb = b - p;
	const glm::vec3 pc = c - p;

	glm::vec3 m = glm::cross(pq, pc);
	double u = glm::dot(pb, m);
	double v = -glm::dot(pa, m);
	if (!same_sign(u, v)) return false;
	double w = scalar_product(pq, pb, pa);
	if (!same_sign(u, w)) return false;

	if (u + v + w == 0) return false;

	const double denom = 1.0 / (u + v + w);
	u *= denom;
	v *= denom;
	w *= denom; // w = 1.0f - u - v;

	collision.x = (u * a.x + v * b.x + w * c.x);
	collision.y = (u * a.y + v * b.y + w * c.y);
	collision.z = (u * a.z + v * b.z + w * c.z);

	return true;
}

/*************************************************
 */
 // http://realtimecollisiondetection.net/blog/?p=103
bool Framework::IntersectLineTriangle(
	const glm::vec3& p1,
	const glm::vec3& p2,
	const glm::vec3& p3,
	const glm::vec3& P,
	float r)
{
	glm::vec3 A = p1 - P;
	glm::vec3 B = p2 - P;
	glm::vec3 C = p2 - P;

	double rr = (float)r * r;
	glm::vec3 V = glm::cross(B - A, C - A);
	double d = glm::dot(A, V);
	double e = glm::dot(V, V);
	int sep1 = d * d > rr * e;

	if (sep1)
		return true;

	double aa = glm::dot(A, A);
	double ab = glm::dot(A, B);
	double ac = glm::dot(A, C);
	int sep2 = (aa > rr) & (ab > aa) & (ac > aa);

	if (sep2)
		return true;

	double bb = glm::dot(B, B);
	double bc = glm::dot(B, C);
	int sep3 = (bb > rr) & (ab > bb) & (bc > bb);

	if (sep3)
		return true;

	double cc = glm::dot(C, C);
	int sep4 = (cc > rr) & (ac > cc) & (bc > cc);

	if (sep4)
		return true;

	glm::vec3 AB = B - A;
	glm::vec3 BC = C - B;
	glm::vec3 CA = A - C;

	float d1 = ab - aa;
	float e1 = glm::dot(AB, AB);

	glm::vec3 Q1 = A * e1 - AB * d1;
	glm::vec3 QC = C * e1 - Q1;
	int sep5 = (glm::dot(Q1, Q1) > rr * e1 * e1) & (glm::dot(Q1, QC) > 0);

	if (sep5)
		return true;

	float d2 = bc - bb;
	float e2 = glm::dot(BC, BC);

	glm::vec3 Q2 = B * e2 - BC * d2;
	glm::vec3 QA = A * e2 - Q2;
	int sep6 = (glm::dot(Q2, Q2) > rr * e2 * e2) & (glm::dot(Q2, QA) > 0);

	if (sep6)
		return true;

	float d3 = ac - cc;
	float e3 = glm::dot(CA, CA);

	glm::vec3 Q3 = C * e3 - CA * d3;
	glm::vec3 QB = B * e3 - Q3;
	int sep7 = (glm::dot(Q3, Q3) > rr * e3 * e3) & (glm::dot(Q3, QB) > 0);

	if (sep7)
		return true;

	return false;
}

//https://www.flipcode.com/archives/Moving_Sphere_VS_Triangle_Collision.shtml
//http://www.ambrsoft.com/TrigoCalc/Sphere/SpherLineIntersection_.htm

bool Framework::IntersectionSphereLine(const glm::vec3& center,
	const float radius,
	const glm::vec3& pt0,
	const glm::vec3& pt1,
	glm::vec3& collision)
{
	float a, b, c, i;

	a = pow(pt1.x - pt0.x, 2) + pow(pt1.y - pt0.y, 2) + pow(pt1.z - pt0.z, 2);
	b = 2 * ((pt1.x - pt0.x) * (pt0.x - center.x)
		+ (pt1.y - pt0.y) * (pt0.y - center.y)
		+ (pt1.z - pt0.z) * (pt0.z - center.z));
	c = pow(center.x, 2) + pow(center.y, 2) +
		pow(center.z, 2) + pow(pt0.x, 2) +
		pow(pt0.y, 2) + pow(pt0.z, 2) -
		2 * (center.x * pt0.x + center.y * pt0.y + center.z * pt0.z) - pow(radius, 2);
	i = b * b - 4 * a * c;

	if (i < 0)
		return false;

	if (i == 0) {	
		// intersection on the sphere
		float t0 = -b / (2 * a);

		// check the collision point is on the segment
		collision=glm::vec3(
			pt0.x + (pt1.x - pt0.x) * t0,
			pt0.y + (pt1.y - pt0.y) * t0,
			pt0.z + (pt1.z - pt0.z) * t0
			);
		float d = glm::dot(pt1 - pt0, pt1 - pt0);
		float v = glm::dot(pt1 - pt0, collision - pt0);
		if (v < 0 || v > d) {
			return false;
		}
	}
	else {			
		// intersection through the sphere
		float t0 = (-b + sqrtf(pow(b, 2) - 4 * a * c)) / (2 * a);
		float t1 = (-b - sqrtf(pow(b ,2) - 4 * a * c)) / (2 * a);

		// check the collision point is on the segment
		glm::vec3 p0(
			pt0.x + (pt1.x - pt0.x) * t0,
			pt0.y + (pt1.y - pt0.y) * t0,
			pt0.z + (pt1.z - pt0.z) * t0
		);
		glm::vec3 p1(
			pt0.x + (pt1.x - pt0.x) * t1,
			pt0.y + (pt1.y - pt0.y) * t1,
			pt0.z + (pt1.z - pt0.z) * t1
		);
		float d = glm::dot(pt1 - pt0, pt1 - pt0);
		float v = glm::dot(pt1 - pt0, p0 - pt0);
		if (v < 0 || v > d) {
			return false;
		}
		float v1 = glm::dot(pt1 - pt0, p1 - pt0);
		if (v1 < 0 || v1 > d) {
			return false;
		}

		collision = (p0 + p1) / 2.0f;
	}

	return true;
}

/**
 * compute the intersection of 2 segments AB et CD
 * https://www.developpez.net/forums/d369370/applications/developpement-2d-3d-jeux/algo-intersection-2-segments/
 */
bool Framework::segment2segment(glm::vec2& A, glm::vec2& B, glm::vec2& C, glm::vec2& D, glm::vec2& result)
{
	float Ax = A.x;
	float Ay = A.y;
	float Bx = B.x;
	float By = B.y;
	float Cx = C.x;
	float Cy = C.y;
	float Dx = D.x;
	float Dy = D.y;

	float Sx;
	float Sy;

	if (Ax == Bx)
	{
		if (Cx == Dx) return false;
		else
		{
			float pCD = (Cy - Dy) / (Cx - Dx);
			Sx = Ax;
			Sy = pCD * (Ax - Cx) + Cy;
		}
	}
	else
	{
		if (Cx == Dx)
		{
			float pAB = (Ay - By) / (Ax - Bx);
			Sx = Cx;
			Sy = pAB * (Cx - Ax) + Ay;
		}
		else
		{
			float pCD = (Cy - Dy) / (Cx - Dx);
			float pAB = (Ay - By) / (Ax - Bx);
			float oCD = Cy - pCD * Cx;
			float oAB = Ay - pAB * Ax;
			Sx = (oAB - oCD) / (pCD - pAB);
			Sy = pCD * Sx + oCD;
		}
	}
	if ((Sx < Ax && Sx < Bx) | (Sx > Ax && Sx > Bx) | (Sx < Cx && Sx < Dx) | (Sx > Cx && Sx > Dx)
		| (Sy < Ay && Sy < By) | (Sy > Ay && Sy > By) | (Sy < Cy && Sy < Dy) | (Sy > Cy && Sy > Dy)) return false;

	result.x = Sx;
	result.y = Sy;
	return true;
}


/**
 * compute the intersection of 1 segment AB and a circle C,r
 * https://stackoverflow.com/questions/1073336/circle-line-segment-move-detection-algorithm
 */
bool Framework::fat_point_collides_segment(glm::vec2& A, glm::vec2& B, glm::vec2& C, float r, glm::vec2& result)
{
	glm::vec2 s0s1 = B - A;
	glm::vec2 s0qp = C - A;
	float rSqr = r * r;

	auto a = glm::dot(s0s1, s0s1);
	//if( a != 0 ) // if you haven't zero-length segments omit this, as it would save you 1 _mm_comineq_ss() instruction and 1 memory fetch
	{
		auto b = glm::dot(s0s1, s0qp);
		auto t = b / a; // length of projection of s0qp onto s0s1
		//std::cout << "t = " << t << "\n";
		if ((t >= 0) && (t <= 1)) // 
		{
			auto c = glm::dot(s0qp, s0qp);
			auto r2 = c - a * t * t;
			if (r2 <= rSqr) {
				result = s0s1 * t + A;
				return true;
			}
		}
	}
	return false;
}

/**
 * return the move point of a circle with a segment in 2D
 */
bool Framework::CircLine(glm::vec2& A, glm::vec2& B, glm::vec2& C, float r, glm::vec2& result)
{
	glm::vec2 AC = C - A;
	glm::vec2 AB = B - A;
	float ab2 = glm::dot(AB, AB);
	float acab = glm::dot(AC, AB);
	float t = acab / ab2;

	if (t < 0.0)
		t = 0.0;
	else if (t > 1.0)
		t = 1.0;

	//P = A + t * AB;
	result = AB * t + A;

	glm::vec2 H = result - C;
	float h2 = glm::dot(H, H);
	float r2 = r * r;

	if (h2 > r2)
		return false;
	else
		return true;
}

/**
 * test if ray intersect with a AABBox
 * https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
 */
bool Framework::intersectRayAABox2(const glm::vec3& p0, const glm::vec3& p1, const fwAABBox& box, float& tnear, float& tfar)
{
	glm::vec3 T_1, T_2, temp; // vectors to hold the T-values for every direction
	double t_near = -DBL_MAX; // maximums defined in float.h
	double t_far = DBL_MAX;

	glm::vec3 direction = p1 - p0;

	if (direction.x == 0) { // ray parallel to planes in this direction
		if ((p0.x < box.m_p.x) || (p0.x > box.m_p1.x)) {
			return false; // parallel AND outside box : no intersection possible
		}
	}
	else { // ray not parallel to planes in this direction
		T_1.x = (box.m_p.x - p0.x) / direction.x;
		T_2.x = (box.m_p1.x - p0.x) / direction.x;

		if (T_1.x > T_2.x) { // we want T_1 to hold values for intersection with near plane
			temp = T_2;
			T_2 = T_1;
			T_1 = temp;
		}
		if (T_1.x > t_near) {
			t_near = T_1.x;
		}
		if (T_2.x < t_far) {
			t_far = T_2.x;
		}
		if ((t_near > t_far) || (t_far < 0)) {
			return false;
		}
	}

	if (direction.y == 0) { // ray parallel to planes in this direction
		if ((p0.y < box.m_p.y) || (p0.x > box.m_p1.y)) {
			return false; // parallel AND outside box : no intersection possible
		}
	}
	else { // ray not parallel to planes in this direction
		T_1.y = (box.m_p.y - p0.y) / direction.y;
		T_2.y = (box.m_p1.y - p0.y) / direction.y;

		if (T_1.y > T_2.y) { // we want T_1 to hold values for intersection with near plane
			temp = T_2;
			T_2 = T_1;
			T_1 = temp;
		}
		if (T_1.y > t_near) {
			t_near = T_1.y;
		}
		if (T_2.y < t_far) {
			t_far = T_2.y;
		}
		if ((t_near > t_far) || (t_far < 0)) {
			return false;
		}
	}

	if (direction.z == 0) { // ray parallel to planes in this direction
		if ((p0.z < box.m_p.z) || (p0.z > box.m_p1.z)) {
			return false; // parallel AND outside box : no intersection possible
		}
	}
	else { // ray not parallel to planes in this direction
		T_1.z = (box.m_p.z - p0.z) / direction.z;
		T_2.z = (box.m_p1.z - p0.z) / direction.z;

		if (T_1.z > T_2.z) { // we want T_1 to hold values for intersection with near plane
			temp = T_2;
			T_2 = T_1;
			T_1 = temp;
		}
		if (T_1.z > t_near) {
			t_near = T_1.z;
		}
		if (T_2.z < t_far) {
			t_far = T_2.z;
		}
		if ((t_near > t_far) || (t_far < 0)) {
			return false;
		}
	}

	tnear = t_near; tfar = t_far; // put return values in place
	return true; // if we made it here, there was an intersection - YAY
}

/**
 * Collision AABB vs Plane
 * https://gist.github.com/yomotsu/d845f21e2e1eb49f647f
 */
bool Framework::PlaneAABB(const fwAABBox& aabb, const fwPlane& plane)
{
	const glm::vec3 center = (aabb.m_p1 + aabb.m_p) * 0.5f;
	const glm::vec3 extents = aabb.m_p1 - center;

	float r = extents.x * abs(plane.normal().x) + extents.y * abs(plane.normal().y) + extents.z * abs(plane.normal().z);
	float s = glm::dot(plane.normal(), center) - plane.constant();

	return abs(s) <= r;
}

/**
 * Collision AABB vs Triangle
 * https://gist.github.com/yomotsu/d845f21e2e1eb49f647f
 */
bool Framework::TriangleAABB(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const fwAABBox& aabb) {

	// Compute box center and extents of AABoundingBox (if not already given in that format)
	glm::vec3 center = (aabb.m_p1 + aabb.m_p) * 0.5f;
	glm::vec3 extents = aabb.m_p1 - center;

	// Translate triangle as conceptually moving AABB to origin
	const glm::vec3 v0 = a - center;
	const glm::vec3 v1 = b - center; 
	const glm::vec3 v2 = c - center;

	// Compute edge vectors for triangle
	const glm::vec3  f0 = v1 - v0;
	const glm::vec3 f1 = v2 - v1;
	const glm::vec3 f2 = v0 - v2;

	// Test axes a00..a22 (category 3)
	const glm::vec3 a00(0, -f0.z, f0.y);
	const glm::vec3 a01(0, -f1.z, f1.y);
	const glm::vec3 a02(0, -f2.z, f2.y);
	const glm::vec3 a10(f0.z, 0, -f0.x);
	const glm::vec3 a11(f1.z, 0, -f1.x);
	const glm::vec3 a12(f2.z, 0, -f2.x);
	const glm::vec3 a20(-f0.y, f0.x, 0);
	const glm::vec3 a21(-f1.y, f1.x, 0);
	const glm::vec3 a22(-f2.y, f2.x, 0);

	// Test axis a00
	float p0 = glm::dot(v0, a00);
	float p1 = glm::dot(v1, a00);
	float p2 = glm::dot(v2, a00);
	float r = extents.y * abs(f0.z) + extents.z * abs(f0.y);


	if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
		return false; // Axis is a separating axis
	}

	// Test axis a01
	p0 = glm::dot(v0, a01);
	p1 = glm::dot(v1, a01);
	p2 = glm::dot(v2, a01);
	r = extents.y * abs(f1.z) + extents.z * abs(f1.y);

	if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
		return false; // Axis is a separating axis
	}

	// Test axis a02
	p0 = glm::dot(v0, a02);
	p1 = glm::dot(v1, a02);
	p2 = glm::dot(v2, a02);
	r = extents.y * abs(f2.z) + extents.z * abs(f2.y);

	if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
		return false; // Axis is a separating axis
	}

	// Test axis a10
	p0 = glm::dot(v0, a10);
	p1 = glm::dot(v1, a10);
	p2 = glm::dot(v2, a10);
	r = extents.x * abs(f0.z) + extents.z * abs(f0.x);

	if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
		return false; // Axis is a separating axis
	}

	// Test axis a11
	p0 = glm::dot(v0, a11);
	p1 = glm::dot(v1, a11);
	p2 = glm::dot(v2, a11);
	r = extents.x * abs(f1.z) + extents.z * abs(f1.x);

	if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
		return false; // Axis is a separating axis
	}

	// Test axis a12
	p0 = glm::dot(v0, a12);
	p1 = glm::dot(v1, a12);
	p2 = glm::dot(v2, a12);
	r = extents.x * abs(f2.z) + extents.z * abs(f2.x);

	if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
		return false; // Axis is a separating axis
	}

	// Test axis a20
	p0 = glm::dot(v0, a20);
	p1 = glm::dot(v1, a20);
	p2 = glm::dot(v2, a20);
	r = extents.x * abs(f0.y) + extents.y * abs(f0.x);

	if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
		return false; // Axis is a separating axis
	}

	// Test axis a21
	p0 = glm::dot(v0, a21);
	p1 = glm::dot(v1, a21);
	p2 = glm::dot(v2, a21);
	r = extents.x * abs(f1.y) + extents.y * abs(f1.x);

	if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
		return false; // Axis is a separating axis
	}

	// Test axis a22
	p0 = glm::dot(v0, a22);
	p1 = glm::dot(v1, a22);
	p2 = glm::dot(v2, a22);
	r = extents.x * abs(f2.y) + extents.y * abs(f2.x);

	if (std::max(-std::max(p0, std::max(p1, p2)), std::min(p0, std::min(p1, p2))) > r) {
		return false; // Axis is a separating axis
	}

	// Test the three axes corresponding to the face normals of AABB b (category 1).
	// Exit if...
	// ... [-extents.x, extents.x] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
	if (std::max(v0.x, std::max(v1.x, v2.x)) < -extents.x || std::min(v0.x, std::min(v1.x, v2.x)) > extents.x) {
		return false;
	}

	// ... [-extents.y, extents.y] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
	if (std::max(v0.y, std::max(v1.y, v2.y)) < -extents.y || std::min(v0.y, std::min(v1.y, v2.y)) > extents.y) {
		return false;
	}
	// ... [-extents.z, extents.z] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
	if (std::max(v0.z, std::max(v1.z, v2.z)) < -extents.z || std::min(v0.z, std::min(v1.z, v2.z)) > extents.z) {
		return false;
	}
	// Test separating axis corresponding to triangle face normal (category 2)
	// Face Normal is -ve as Triangle is clockwise winding (and XNA uses -z for into screen)
	glm::vec3 normal = glm::normalize(glm::cross(f1, f0));
	fwPlane plane (
		normal,
		glm::dot(normal, a)
		);

	return PlaneAABB(aabb, plane);
}

