#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <vector>
#include <array>

#include "../framework/math/fwSegment2D.h"
#include "../framework/math/fwAABBox2D.h"

// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
// data won't have more than 65536 vertices.
using N = uint32_t;
using Coord = float;
using Point = std::array<Coord, 2>;

namespace GameEngine {
	/**
	 * portals connect triangles together through edges
	 */
	class satNavPortal {
		int32_t m_triangle = -1;								// index of the neighbor triangles
		glm::vec2 m_center;										// center of the edge
		glm::vec2 m_normal;										// normal of the portal
		int32_t m_edges[2] = { -1,-1 };							// index of the vertices making the portal (portal p, vertices p*2 and p*2+1)
		float m_dist = -1;										// distance from the 2 centers through the portal
		friend class NavMesh;
		friend class satNavTriangle;
	};

	/**
	 * extend a triangle with portals and normals
	 */
	class satNavTriangle {
		uint32_t m_index;										// index of the triangle in the list
		glm::vec2 m_vertices[3];
		glm::vec3 m_center;
		satNavPortal m_portals[3];

		Framework::AABBox2D m_aabb;

		friend class NavMesh;
	public:
		satNavTriangle(const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3, float z);
		satNavTriangle();
		int32_t addPortal(uint32_t t);
		int32_t findPortal(const Framework::Segment2D& line);	// return a portal crossed by line
		bool inside(const glm::vec2& p);						// is point in triangle
	};

	/**
	 *
	 */
	class NavMesh {
		std::vector<satNavTriangle> m_triangles;
		bool m_debug = false;

		int32_t findTriangle(const glm::vec3& p, float z, float z1);// find the nearest triangle to the position
		bool lineOfSight(
			const Framework::Segment2D& line,
			satNavTriangle* from, satNavTriangle* to);			// is there a direct line of sight (x,y) -> (x1,y1) over the triangles
		bool findDirectPath(uint32_t from, 
			uint32_t to,
			std::vector<glm::vec3>& graphPath,
			std::vector<glm::vec3>& directPath);				// build a new direct path from->to using the graph path

	public:
		void addFloor(std::vector<std::vector<Point>>& polygons, float z);
		void buildMesh(void);
		float findPath(const glm::vec3& from, 
			const glm::vec3& to,
			std::vector<glm::vec3>& path);						// build a path from/to
	};
}

extern GameEngine::NavMesh g_navMesh;