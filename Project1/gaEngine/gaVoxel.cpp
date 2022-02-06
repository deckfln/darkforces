#include "gaVoxel.h"

/**
 *
 */
GameEngine::Voxel::Voxel(const glm::vec3& pmin, const glm::vec3& pmax, uint32_t level):
	m_level(level)
{
	m_aabb.set(pmin, pmax);
}

/**
 *
 */
void GameEngine::Voxel::set(const glm::vec3& pmin, const glm::vec3& pmax, uint32_t level)
{
	m_level = level;
	m_aabb.set(pmin, pmax);
	for (size_t i = 0; i < 8; i++) {
		m_blocks[i] = -1;
	}
}
