#include "gaNavMesh.h"

#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <poly2tri/poly2tri.h>

#include <map>
#include <iostream>
#include <queue>
#include <deque>
#include <vector>
#include <functional>

#include "../include/earcut.hpp"

GameEngine::NavMesh g_navMesh;

const float my_EPSILON = 4 * FLT_EPSILON;

/**
 * // find the nearest triangle to the position
 */
int32_t GameEngine::NavMesh::findTriangle(const glm::vec3& p, float z, float z1)
{
	glm::vec2 p2D(p.x, p.z);
	for (uint32_t i = 0; i < m_triangles.size(); i++) {
		float d = abs(p.y - m_triangles[i].m_center.y);

		if (m_triangles[i].inside(p2D)) {
			if (d <= 8) {
				return i;
			}
		}
	}

	return -1;
}

int32_t GameEngine::NavMesh::findTriangle(const glm::vec2& p, float z)
{
	for (uint32_t i = 0; i < m_triangles.size(); i++) {
		float d = abs(z - m_triangles[i].m_center.y);

		if (m_triangles[i].inside(p)) {
			if (d <= 8) {
				return i;
			}
		}
	}

	return -1;
}

/**
 *
 */
bool GameEngine::NavMesh::crossTriangles(const Framework::Segment2D& line, float Fromy, float Toy)
{
	int32_t trFrom = findTriangle(line.start(), Fromy);
	if (trFrom < 0)
		return false;
	int32_t trTo = findTriangle(line.end(), Toy);
	if (trTo < 0)
		return false;


	satNavTriangle* current = &m_triangles[trFrom];
	satNavTriangle* to = &m_triangles[trTo];

	int32_t portal, portal1;
	std::vector<uint32_t> previousTriangles;

	while (current != to) {
		/*
		for (uint32_t i = 0; i < 3; i++) {
			printf("triangle:%.0f,%.0f, ",
				current->m_vertices[i].x,
				current->m_vertices[i].y);
		}
		printf("\n");
		*/

		// test left and right line
		portal = current->findPortal(line);
		if (portal < 0) {
			return false;
		}
		current = &m_triangles[portal];
	}

	return true;

}

/**
 * 
 */
bool GameEngine::NavMesh::checkSegment(const glm::vec2& from, const glm::vec2& to, float Fromy, float Toy, float radius)
{
	const glm::vec2 d = glm::normalize(to - from);

	Framework::Segment2D left(
		from + glm::vec2(-d.y, d.x) * radius,
		to + glm::vec2(-d.y, d.x) * radius
	);

	if (!crossTriangles(left, Fromy, Toy)) {
		return false;
	}

	Framework::Segment2D right(
		from + glm::vec2(+d.y, -d.x) * radius,
		to + glm::vec2(+d.y, -d.x) * radius
	);

	if (!crossTriangles(right, Fromy, Toy)) {
		return false;
	}

	return true;
}

/**
 * is there a direct line of sight (x,y) -> (x1,y1) over the triangles
 */
bool GameEngine::NavMesh::lineOfSight(const Framework::Segment2D& line, satNavTriangle* from, satNavTriangle* to)
{
	satNavTriangle* current = from;
	int32_t portal, portal1;
	std::vector<uint32_t> previousTriangles;

	// extend the start and the end of the line on a circle
	glm::vec2 d = line.end() - line.start();
	d = glm::normalize(d);

	Framework::Segment2D left(
		line.start() + glm::vec2(-d.y, d.x) * 0.2f,
		line.end() + glm::vec2(-d.y, d.x) * 0.2f
		);
	Framework::Segment2D right(
		line.start() + glm::vec2(+d.y, -d.x) * 0.2f,
		line.end() + glm::vec2(+d.y, -d.x) * 0.2f
	);

	while (current != to) {
		/*
		for (uint32_t i = 0; i < 3; i++) {
			printf("triangle:%.0f,%.0f, ",
				current->m_vertices[i].x,
				current->m_vertices[i].y);
		}
		printf("\n");
		*/

		// test left and right line
		portal = current->findPortal(left);
		if (portal < 0) {
			return false;
		}
		portal1 = current->findPortal(right);
		if (portal1 < 0) {
			return false;
		}
		if (portal != portal1) {
			__debugbreak();
		}
		current = &m_triangles[portal];
	}

	return true;
}

/**
 * build a new direct path from->to using the graph path
 */
bool GameEngine::NavMesh::findDirectPath(uint32_t from, uint32_t to,
	float radius,
	std::vector<navpoint>& graphPath)
{
	// try to find a direct path from,to [ converted to 2D ]
	glm::vec3 from3D = graphPath[from].p * 10.0f;
	glm::vec3 to3D = graphPath[to].p * 10.0f;

	glm::vec2 from2D(from3D.x, from3D.z);
	glm::vec2 to2D(to3D.x, to3D.z);

	if (checkSegment(from2D, to2D, from3D.y, to3D.y, radius*10.0f)) {
		/*
		printf("[\"sucess\", %f,%f,%f,%f],\n",
			from2D.x / 10.0f, from2D.y / 10.0f,
			to2D.x / 10.0f, to2D.y / 10.0f);
		*/
		graphPath[from].next = to;
		return true;
	}

	// else split the graph in half and try to build 2 direct paths
	uint32_t mid = from + (to - from) / 2;

	if (mid - from == 1) {
		graphPath[from].next = to;
	}
	else {
		/*
		printf("[\"fails\", %f,%f,%f,%f],\n",
			from2D.x/10.0f, from2D.y / 10.0f,
			to2D.x / 10.0f, to2D.y / 10.0f);
		*/
		findDirectPath(from, mid, radius, graphPath);
		findDirectPath(mid, to, radius, graphPath);
	}
	return false;
}

/**
 *
 */
void GameEngine::NavMesh::addFloor(std::vector<std::vector<Point>>& polygons, float z)
{
	// poly2tri creates a nicer layout for a navmesh
	std::vector<p2t::Point> _polyline(polygons[0].size());
	std::vector<p2t::Point*> polyline(polygons[0].size());

	uint32_t i = 0;

	for (auto& p : polygons[0]) {
		_polyline[i] = p2t::Point(p[0], p[1]);
		polyline[i] = &_polyline[i];
		i++;
	}

	p2t::CDT cdt(polyline);

	std::vector<p2t::Point> _hole(0);
	std::vector<p2t::Point*> hole(0);

	if (polygons.size() > 1) {
		_hole.resize(polygons[1].size());
		hole.resize(polygons[1].size());

		i = 0;
		for (auto& p : polygons[1]) {
			_hole[i] = p2t::Point(p[0], p[1]);
			hole[i] = &_hole[i];
			i++;
		}
		cdt.AddHole(hole);
	}

	cdt.Triangulate();
	std::vector<p2t::Triangle*> triangles = cdt.GetTriangles();

	uint32_t start = m_triangles.size();
	m_triangles.resize(m_triangles.size() + triangles.size());

	for (uint32_t i = 0; i < triangles.size(); i ++) {
		p2t::Point* t1 = triangles[i]->GetPoint(0);
		p2t::Point* t2 = triangles[i]->GetPoint(1);
		p2t::Point* t3 = triangles[i]->GetPoint(2);

		m_triangles[start] = satNavTriangle(
			glm::vec2(t1->x, t1->y),
			glm::vec2(t2->x, t2->y),
			glm::vec2(t3->x, t3->y),
			z
		);
		m_triangles[start].m_index = start;

		start++;
	}
}

/**
 * brute force method to build the nav-mesh by looking to adjacent triangles
 */
void GameEngine::NavMesh::buildMesh(void)
{
	// connect the triangles by shared edges
	satNavTriangle* first;
	satNavTriangle* second;
	float len;
	glm::vec2 edge_center;
	glm::vec2 normal;

	for (uint32_t f = 0; f < m_triangles.size(); f++) {
		for (uint32_t s = f + 1; s < m_triangles.size(); s++) {

			first = &m_triangles[f];
			second = &m_triangles[s];

			for (uint32_t i = 0; i < 3; i++) {
				uint32_t iplus = (i == 2) ? 0 : i + 1;

				for (int32_t j = 2; j >= 0; j--) {
					uint32_t jminus = (j == 0) ? 2 : j - 1;

					if (
						first->m_vertices[i] == second->m_vertices[j] &&
						first->m_vertices[iplus] == second->m_vertices[jminus] &&
						abs(first->m_center.y - second->m_center.y) <= 2.06f
						) {
						len = glm::distance(first->m_center, second->m_center);

						edge_center = (first->m_vertices[i] + first->m_vertices[iplus]) / 2.0f;

						normal.x = -first->m_vertices[i].y + first->m_vertices[iplus].y;
						normal.y = first->m_vertices[i].x - first->m_vertices[iplus].x;

						first->m_portals[i].m_triangle = s;
						first->m_portals[i].m_dist = len;
						first->m_portals[i].m_center = edge_center;
						first->m_portals[i].m_edges[0] = i;
						first->m_portals[i].m_edges[1] = iplus;
						first->m_portals[i].m_normal = normal;

						second->m_portals[jminus].m_triangle = f;
						second->m_portals[jminus].m_dist = len;
						second->m_portals[jminus].m_center = edge_center;
						second->m_portals[jminus].m_edges[0] = j;
						second->m_portals[jminus].m_edges[1] = jminus;
						second->m_portals[jminus].m_normal = -normal;
					}
				}
			}

		}
	}
	/*
	for (uint32_t i = 0; i < m_triangles.size(); i++) {
		printf("%d,%.02f, %.0f,%.0f,%.0f,%.0f,%.0f,%.0f\n",
			i,
			m_triangles[i].m_center.y,
			m_triangles[i].m_vertices[0].x,
			m_triangles[i].m_vertices[0].y,
			m_triangles[i].m_vertices[1].x,
			m_triangles[i].m_vertices[1].y,
			m_triangles[i].m_vertices[2].x,
			m_triangles[i].m_vertices[2].y		
		);
	}
	*/
}

/**
 * build a path from/to
 */
class Node {
public:
	uint32_t triangle=-1;
	float distance=0;
	Node(int32_t t, float d) { triangle = t; distance = d; }
	Node(void) {};
};

//Overload the < operator.
bool operator< (const Node& node1, const Node& node2)
{
	return node1.distance < node2.distance;
}
//Overload the > operator.
bool operator> (const Node& node1, const Node& node2)
{
	return node1.distance > node2.distance;
}


float GameEngine::NavMesh::findPath(gaEntity* entity, const glm::vec3& to, std::vector<glm::vec3>& directPath)
{
	std::vector<navpoint> graphPath;		// path computed from the navmesh graph

	const glm::vec3& from = entity->position();
	glm::vec3 from3D = from * 10.0f;
	glm::vec3 to3D = to * 10.0f;

	int32_t start = findTriangle(from3D, from3D.y, to3D.y);
	int32_t end = findTriangle(to3D, from3D.y, to3D.y);

	if (start < 0 || end < 0) {
		return -1;
	}

	// A* algorithm
	// https://www.redblobgames.com/pathfinding/a-star/introduction.html

	std::priority_queue<Node, std::vector<Node>, std::greater<Node> > frontier;

	frontier.push(Node(start, 0));

	std::map<uint32_t, int32_t> came_from;
	std::map<uint32_t, glm::vec2> came_from_portal;
	std::map<uint32_t, float> cost_so_far;

	cost_so_far[start] = 0;

	int32_t current = start;
	int32_t next;
	Node c;
	float new_cost;
	float priority;

	while (!frontier.empty()) {
		c = frontier.top();
		frontier.pop();

		current = c.triangle;

		if (current == end) {
			break;
		}

		for (int32_t i = 0; i < 3; i++) {
			next = m_triangles[current].m_portals[i].m_triangle;
			if (next < 0) {
				continue;
			}

			new_cost = cost_so_far[current] + m_triangles[current].m_portals[i].m_dist;
			if (cost_so_far.count(next) == 0 || new_cost < cost_so_far[next]) {
				cost_so_far[next] = new_cost;
				priority = new_cost + glm::distance(m_triangles[end].m_center, m_triangles[next].m_center);
				frontier.push(Node(next, priority));
				came_from[next] = current;

				came_from_portal[next] = m_triangles[current].m_portals[i].m_center;
			}
		}
	}

	if (current != end) {
		return -1;
	}

	// back track from end to start
	graphPath.push_back({ to, -1 });
	glm::vec2 portal;
	float len = 0;
	glm::vec3 from1 = to, to1;

	while (current != start) {
		current = came_from[current];
		portal = came_from_portal[current];

		if (current != start) {
			to1 = glm::vec3(portal.x, m_triangles[current].m_center.y, portal.y);
			//to1 = m_triangles[current].m_center;

			//path.push_back(glm::vec3(portal.x, m_triangles[current].m_center.y, portal.y) / 10.0f);
			graphPath.push_back({ to1 / 10.0f, -1 });

			len += glm::distance(from1, to1);

			from1 = to1;
		}
	};

	len += glm::distance(from1, from);
	graphPath.push_back({ from, -1 });
	/*
	printf("gaNavMesh::unoptimized path\n");
	for (auto& n : graphPath) {
		printf("%f,%f,\n", n.p.x, n.p.z);
	}
	printf("gaNavMesh::unoptimized path\n");
	*/
	// and now optimize the path using direct paths
	//printf("gaNavMesh::optimize\n");
	findDirectPath(0, graphPath.size() - 1, entity->radius(), graphPath);
	//printf("gaNavMesh::optimize\n");
	int32_t n = 0;
	while (n >= 0) {
		directPath.push_back(graphPath[n].p);
		n = graphPath[n].next;
	}
	
	printf("gaNavMesh::findPath\n");
	for (auto& p : directPath) {
		printf("%f,%f,\n", p.x, p.z);
	}
	printf("<\n");

	return len;
}

/**
 * manage nav triangles
 */

static uint32_t g_index = 0;

GameEngine::satNavTriangle::satNavTriangle(const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3, float z)
{
	m_vertices[0] = t1;
	m_vertices[1] = t2;
	m_vertices[2] = t3;

	glm::vec2 c = (t1 + t2 + t3) / 3.0f;
	m_center = glm::vec3(c.x, z, c.y);

	m_aabb.set(&m_vertices[0], 3);
}

GameEngine::satNavTriangle::satNavTriangle()
{
}

int32_t GameEngine::satNavTriangle::addPortal(uint32_t t)
{
	int32_t f = -1;
	for (int32_t i = 0; i < 3; i++) {
		if (m_portals[i].m_triangle == -1) {
			m_portals[i].m_triangle = t;
			f = i;
			break;
		}
	}

	if (f < 0) {
		__debugbreak();
	}

 	return f;
}

/**
 * return a portal crossed by line
 */
int32_t GameEngine::satNavTriangle::findPortal(const Framework::Segment2D& line)
{
	Framework::Segment2D edge;
	uint32_t v, v1;	// vertices
	int32_t nextTriangle;
	glm::vec2 intersection;
	const glm::vec2& start = line.start();
	const glm::vec2& end = line.end();

	/*
	for (uint32_t i = 0; i < 3; i++) {
		printf("normal:%f,%f,%f,%f\n",
			m_portals_p[i].x, m_portals_p[i].y,
			m_portals_p[i].x + m_normal[i].x, m_portals_p[i].y + m_normal[i].y);
	}
	*/

	// test each edge
	for (uint32_t i = 0; i < 3; i++) {
		nextTriangle = m_portals[i].m_triangle;
		// there is actually a portal on that edge
		if (nextTriangle >= 0) {

			// do not test portal looking backward
			if (glm::dot(line.direction(), m_portals[i].m_normal) <= 0) {
				continue;
			}

			v = m_portals[i].m_edges[0];
			v1 = m_portals[i].m_edges[1];
			edge.set(m_vertices[v], m_vertices[v1]);

			if (edge.intersect(line, intersection)) {
				//printf(">%f,%f\n", m_portals_p[i].x, m_portals_p[i].y);
				return nextTriangle;
			}
		}
	}
	return -1;
}

/**
 * is point in triangle
 */
bool GameEngine::satNavTriangle::inside(const glm::vec2& p)
{
	// broadtest against AABB
	if (!m_aabb.inside(p)) {
		return false;
	}

	//http://koozdra.wordpress.com/2012/06/27/javascript-is-point-in-triangle/
	//credit: http://www.blackpawn.com/texts/pointinpoly/default.html
	glm::vec2& a = m_vertices[0];
	glm::vec2& b = m_vertices[1];
	glm::vec2&  c = m_vertices[2];

	glm::vec2 v0 = c - a;;
	glm::vec2 v1 = b - a;
	glm::vec2 v2 = p - a;

	float dot00 = (v0[0] * v0[0]) + (v0[1] * v0[1]);
	float dot01 = (v0[0] * v1[0]) + (v0[1] * v1[1]);
	float dot02 = (v0[0] * v2[0]) + (v0[1] * v2[1]);
	float dot11 = (v1[0] * v1[0]) + (v1[1] * v1[1]);
	float dot12 = (v1[0] * v2[0]) + (v1[1] * v2[1]);

	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);

	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return ((u >= 0) && (v >= 0) && (u + v <= 1));
}
