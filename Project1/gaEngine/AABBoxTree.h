#pragma once

#include <vector>
#include <glm/vec3.hpp>

#include "../framework/fwAABBox.h"

namespace GameEngine {
	class AABBoxTree : public fwAABBox 
	{
		std::vector<AABBoxTree*> m_children;
		glm::vec3 const* m_vertices = nullptr;
		uint32_t m_nbVertices=0;
	public:
		AABBoxTree();
		AABBoxTree(const glm::vec3&, const glm::vec3&);
		AABBoxTree& add(AABBoxTree*child);
		bool find(const fwAABBox& box, std::vector<AABBoxTree *>& results);
		inline glm::vec3 const* vertices(void) { return m_vertices; }
		inline uint32_t nbVertices(void) { return m_nbVertices; };
		inline void geometry(glm::vec3 const* vertices, int nbVertices) { m_vertices = vertices; m_nbVertices = nbVertices; };

		void* m_extra = nullptr;
	};
}