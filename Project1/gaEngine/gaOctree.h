#pragma once

#include <vector>
#include "gaVoxelSpace.h"

namespace GameEngine {
	template <typename T>
	class Octree {
		std::vector<std::vector<T>> m_objects;
		VoxelSpace<std::vector<T>> m_voxel;
	public:
		Octree(int32_t extend, uint32_t depth);

		void add(const glm::vec3& p, T object);
		void* find(const glm::vec3& p);
	};
}