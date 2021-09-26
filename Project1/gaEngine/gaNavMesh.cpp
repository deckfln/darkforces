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

/**
 * // find the nearest triangle to the position
 */
uint32_t GameEngine::NavMesh::findTriangle(const glm::vec3& p)
{
	int32_t nearest_i = 0;
	glm::vec3 nearest = m_triangles[0].m_center;
	float l = glm::distance(nearest, p);
	float l1;

	for (uint32_t i = 1; i < m_triangles.size(); i++) {
		l1 = glm::distance(m_triangles[i].m_center, p);
		if (l1 < l) {
			nearest_i = i;
			l = l1;
		}
	}

	return nearest_i;
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

		m_triangles[start] = Triangle(
			glm::vec2(t1->x, t1->y),
			glm::vec2(t2->x, t2->y),
			glm::vec2(t3->x, t3->y),
			z
		);

		start++;
	}

	// keep earcut for reference
	/*
	// Run tessellation
	// Returns array of indices that refer to the vertices of the input polygon.
	// e.g: the index 6 would refer to {25, 75} in this example.
	// Three subsequent indices form a triangle. Output triangles are clockwise.
	std::vector<N> indices = mapbox::earcut<N>(polygons);

	// index the indexes IN the poly-lines of polygon 
	std::vector<Point> vertices;

	for (auto& poly : polygons) {
		for (auto& p : poly) {
			vertices.push_back(p);
		}
	}

	start = m_triangles.size();
	m_triangles.resize(m_triangles.size() + indices.size() / 3);

	for (uint32_t i = 0; i < indices.size(); i+=3) {
		int t1 = indices[i];
		int t2 = indices[i + 1];
		int t3 = indices[i + 2];

		m_triangles[start] = Triangle(
			glm::vec2(vertices[t1][0], vertices[t1][1]),
			glm::vec2(vertices[t2][0], vertices[t2][1]),
			glm::vec2(vertices[t3][0], vertices[t3][1]),
			z
		);

		start++;
	}
	*/
}

/**
 * brute force method to build the nav-mesh by looking to adjacent triangles
 */
void GameEngine::NavMesh::buildMesh(void)
{
	// connect the triangles by shared edges
	Triangle* first;
	Triangle* second;
	int32_t p;
	float len;

	for (uint32_t f = 0; f < m_triangles.size(); f++) {
		for (uint32_t s = f + 1; s < m_triangles.size(); s++) {

			first = &m_triangles[f];
			second = &m_triangles[s];

			for (uint32_t i = 0; i < 3; i++) {
				uint32_t iplus = (i == 2) ? 0 : i + 1;

				for (int32_t j = 2; j >= 0; j--) {
					uint32_t jminus = (j == 0) ? 2 : j - 1;

					if (
						first->m_edges[i] == second->m_edges[j] &&
						first->m_edges[iplus] == second->m_edges[jminus] &&
						abs(first->m_center.y - second->m_center.y) <= 2.06f
						) {
						len = glm::distance(first->m_center, second->m_center);

						first->m_portals[i] = s;
						first->m_dist[i] = len;
						first->m_portals_p[i] = (first->m_edges[i] + first->m_edges[iplus])/2.0f;

						second->m_portals[jminus] = f;
						second->m_dist[jminus] = len;
						second->m_portals_p[jminus] = (first->m_edges[i] + first->m_edges[iplus]) / 2.0f;
					}
				}

			}

		}
	}

	/*
	for (uint32_t i = 0; i < m_triangles.size(); i++) {
		printf("%d,%.02f, %.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%d,%d,%d\n",
			i,
			m_triangles[i].m_center.y,
			m_triangles[i].m_edges[0].x,
			m_triangles[i].m_edges[0].y,
			m_triangles[i].m_edges[1].x,
			m_triangles[i].m_edges[1].y,
			m_triangles[i].m_edges[2].x,
			m_triangles[i].m_edges[2].y,
			m_triangles[i].m_portals[0],
			m_triangles[i].m_portals[1],
			m_triangles[i].m_portals[2]
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


float GameEngine::NavMesh::findPath(const glm::vec3& from, const glm::vec3& to, std::vector<glm::vec3>& path)
{
	uint32_t start = findTriangle(from * 10.0f);
	uint32_t end = findTriangle(to * 10.0f);

	// A* algorithm
	// https://www.redblobgames.com/pathfinding/a-star/introduction.html

	std::priority_queue<Node, std::vector<Node>, std::greater<Node> > frontier;

	frontier.push(Node(start, 0));

	std::map<uint32_t, int32_t> came_from;
	std::map<uint32_t, glm::vec2> came_from_portal;
	std::map<uint32_t, float> cost_so_far;

	cost_so_far[start] = 0;

	int32_t current, next;
	Node c;
	float new_cost;
	float priority;
	glm::vec2 p, p1;

	while (!frontier.empty()) {
		c = frontier.top();
		frontier.pop();

		current = c.triangle;

		if (current == end) {
			break;
		}

		for (int32_t i = 0; i < 3; i++) {
			next = m_triangles[current].m_portals[i];
			if (next < 0) {
				continue;
			}

			new_cost = cost_so_far[current] + m_triangles[current].m_dist[i];
			if (cost_so_far.count(next) == 0 || new_cost < cost_so_far[next]) {
				cost_so_far[next] = new_cost;
				priority = new_cost + glm::distance(m_triangles[end].m_center, m_triangles[next].m_center);
				frontier.push(Node(next, priority));
				came_from[next] = current;

				came_from_portal[next] = m_triangles[current].m_portals_p[i];;
			}
		}
	}

	if (current != end) {
		return -1;
	}

	// back track from end to start
	path.push_back(to);
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
			path.push_back( to1 / 10.0f);

			len += glm::distance(from1, to1);

			from1 = to1;
		}
	};

	len += glm::distance(from1, from);
	path.push_back(from);

	/*
	for (auto& p : path)
		printf("(%.0f,%.0f),\n", p.x * 10.0f, p.z * 10.0f);
	*/
	printf("*\n");

	return len;
}

/**
 *
 */

static uint32_t g_index = 0;

GameEngine::Triangle::Triangle(const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3, float z)
{
	m_edges[0] = t1;
	m_edges[1] = t2;
	m_edges[2] = t3;

	glm::vec2 c = (t1 + t2 + t3) / 3.0f;
	m_center = glm::vec3(c.x, z, c.y);
}

GameEngine::Triangle::Triangle()
{
}

int32_t GameEngine::Triangle::addPortal(uint32_t t)
{
	int32_t f = -1;
	for (int32_t i = 0; i < 3; i++) {
		if (m_portals[i] == -1) {
			m_portals[i] = t;
			f = i;
			break;
		}
	}

	if (f < 0) {
		__debugbreak();
	}

	return f;
}
