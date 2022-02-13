#pragma once

#include <vector>
#include <stack>
#include "gaVoxel.h"

namespace GameEngine {
	class Octree {
		struct node {
			uint32_t m_leaf = 0;
			//		uint32_t m_level=0;
#ifdef _DEBUG
			fwAABBox m_aabb;
#endif
			std::vector<void*> m_objects;

			// 8 subblocks, by default all empty
			int32_t m_blocks[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };
		};

		struct quadrant {
			uint32_t index;
			fwAABBox& aabb;
			uint32_t depth;
			inline quadrant(uint32_t i, fwAABBox& a, uint32_t d) :
				index(i),
				aabb(a),
				depth(d)
			{}
		};

		uint32_t m_depth = 0;
		std::vector<node> m_quadrant;
		std::stack<uint32_t> m_freeList;
		std::stack<quadrant> m_queue;


		void mqueue(
			uint32_t depth,
			fwAABBox& object,
			fwAABBox& subtree,
			glm::vec3& pmin,
			glm::vec3& pmax,
			uint32_t currentNode,
			uint32_t i);

		void merge(int32_t current);
		void release(int32_t index);


	public:
		Octree(int32_t extend, uint32_t depth);

		void add(fwAABBox& aabb, void* object);
		const std::vector<void*>& find(const glm::vec3& p);
		void compress(void);
#ifdef _DEBUG
		void debug(void);
#endif
	};
}