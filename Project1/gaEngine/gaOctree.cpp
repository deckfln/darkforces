#include "gaOctree.h"

template<typename T>
GameEngine::Octree<T>::Octree(int32_t extend, uint32_t depth):
	m_voxel(extend, depth)
{

}

template<typename T>
void GameEngine::Octree<T>::add(const glm::vec3& p, T object)
{
}

template<typename T>
void* GameEngine::Octree<T>::find(const glm::vec3& p)
{
	return nullptr;
}
