#pragma once

#include <vector>
#include "gaVoxel.h"
namespace GameEngine
{
	class VoxelSpace
	{
		std::vector<Voxel> m_voxels;
		int32_t m_freeList = -1;
		uint32_t m_depth = 1;

		int32_t allocate(const glm::vec3& pmin, const glm::vec3& pmax, uint32_t level);
		void release(int32_t voxel);
		void split(uint32_t current);
		void merge(int32_t voxel);

	public:
		VoxelSpace(int32_t extend, uint32_t depth);

		void add(const glm::vec3& p, void* object);
		void* find(const glm::vec3& p);
#ifdef _DEBUG
		void debug(void);
#endif
	};
}
