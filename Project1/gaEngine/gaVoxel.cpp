#include "gaVoxel.h"

/**
 *
 */
template <class T>
GameEngine::Voxel<T>::Voxel(const glm::vec3& pmin, const glm::vec3& pmax, uint32_t level):
	m_level(level)
{
	m_aabb.set(pmin, pmax);
}

/**
 *
 */
template <class T>
void GameEngine::Voxel<T>::set(const glm::vec3& pmin, const glm::vec3& pmax, uint32_t level)
{
	m_level = level;
	m_aabb.set(pmin, pmax);
	for (size_t i = 0; i < 8; i++) {
		m_blocks[i] = -1;
	}
}
