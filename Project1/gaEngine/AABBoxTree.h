#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <string>

#include "../framework/fwAABBox.h"

namespace GameEngine {
	class AABBoxTree : public fwAABBox 
	{
		std::vector<AABBoxTree*> m_children;
		std::vector<glm::vec3>* m_pVertices = nullptr;
		uint32_t m_firstVertice = 0;
		uint32_t m_nbVertices=0;
#ifdef _DEBUG
		std::string m_name;
#endif

	public:
		AABBoxTree();
		AABBoxTree(const glm::vec3&, const glm::vec3&);
		AABBoxTree& add(AABBoxTree*child);

		bool find(const fwAABBox& box, std::vector<AABBoxTree *>& results);

		// getter/setter
#ifdef _DEBUG
		inline const std::string& name(void) { return m_name; };
		inline void name(const std::string& name) { m_name = name; };
#endif

		inline glm::vec3* vertices(void) { return &(*m_pVertices)[m_firstVertice]; }
		inline uint32_t nbVertices(void) { return m_nbVertices; };
		inline uint32_t firstVertice(void) { return m_firstVertice; };
		inline void geometry(std::vector<glm::vec3>* pVertices, int start, int nbVertices) {
			m_pVertices = pVertices;
			m_firstVertice = start; 
			m_nbVertices = nbVertices; 
		};
	};
}