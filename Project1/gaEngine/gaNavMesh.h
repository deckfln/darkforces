#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <vector>
#include <array>

// The index type. Defaults to uint32_t, but you can also pass uint16_t if you know that your
// data won't have more than 65536 vertices.
using N = uint32_t;
using Coord = float;
using Point = std::array<Coord, 2>;

namespace GameEngine {
	class Triangle {
		glm::vec2 m_edges[3];
		glm::vec3 m_center;
		int32_t m_portals[3] = { -1, -1, -1 };
		glm::vec2 m_portals_p[6];
		float m_dist[3] = { -1, -1, -1 };

		friend class NavMesh;
	public:
		Triangle(const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3, float z);
		Triangle();
		int32_t addPortal(uint32_t t);
	};

	class NavMesh {
		std::vector<Triangle> m_triangles;
		uint32_t findTriangle(const glm::vec3& p);					// find the nearest triangle to the position

	public:
		void addFloor(std::vector<std::vector<Point>>& polygons, float z);
		void buildMesh(void);
		bool findPath(const glm::vec3& from, 
			const glm::vec3& to,
			std::vector<glm::vec3>& path);							// build a path from/to
	};
}

extern GameEngine::NavMesh g_navMesh;