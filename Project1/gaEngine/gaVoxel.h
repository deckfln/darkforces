#pragma once

#include "../framework/fwAABBox.h"

namespace GameEngine
{
	template <class T>
	class VoxelSpace;

	template <typename T>
	class Voxel
	{
		int32_t m_nextBlock = -1;
		uint32_t m_level = 0;		// current level of voxel in the octree
#ifdef _DEBUG
		fwAABBox m_aabb;
#endif
		T m_object;

		// 8 subblocks, by default all empty
		int32_t m_blocks[8] = {-1, -1, -1, -1, -1, -1, -1, -1};

		friend GameEngine::VoxelSpace<T>;

	public:
		Voxel(void) {};
		Voxel(const glm::vec3& pmin, const glm::vec3& pmax, uint32_t level);

		void set(const glm::vec3& pmin, const glm::vec3& pmax, uint32_t level);

	};
}