#include "gaVoxelSpace.h"

#include <stack>

#ifdef _DEBUG
#include "gaBoundingBoxes.h"
#endif

/**
 *
 */
int32_t GameEngine::VoxelSpace::allocate(const glm::vec3& pmin, const glm::vec3& pmax, uint32_t level)
{
	int32_t voxel = -1;
	if (m_freeList == -1) {
		m_voxels.push_back(Voxel(pmin, pmax, level));
		voxel = m_voxels.size() - 1;
	}
	else {
		voxel = m_freeList;
		m_freeList = m_voxels[voxel].m_nextBlock;
		m_voxels[voxel].set(pmin, pmax, level);
	}
	return voxel;
}

/**
 *
 */
void GameEngine::VoxelSpace::release(int32_t voxel)
{
	m_voxels[voxel].m_nextBlock = m_freeList;
	m_voxels[voxel].m_object = nullptr;
	m_freeList = voxel;
}

/**
 *
 */
GameEngine::VoxelSpace::VoxelSpace(int32_t extend, uint32_t depth) :
	m_depth(depth)
{
	const glm::vec3 pmin(-extend, -extend, -extend);
	const glm::vec3 pmax(extend, extend, extend);

	m_voxels.push_back(Voxel(pmin, pmax, 0));
}

void GameEngine::VoxelSpace::add(const glm::vec3& p, void* object)
{
	int32_t current = 0;
	int32_t child = -1;
	std::stack<int32_t> history;
	uint32_t depth = 0;

	while (depth < m_depth) {
		history.push(current);

		// if the current voxel is a merge of lower voxels, drop out if the value to store is the same as the value below
		if (m_voxels[current].m_object != nullptr) {
			/*
			if (m_voxels[current].m_object == object) {
				__debugbreak();
				break;
			}
			else {
				// want add a value that is not the merged one, will need to split the lower voxels
				__debugbreak();
			}
			*/
			break;
		}

		int32_t index;
		const glm::vec3& center = m_voxels[current].m_aabb.center();

		// find the correct subblock
		if (p.x > center.x) {
			if (p.y > center.y) {
				if (p.z > center.z) {
					index = 0;
				}
				else {
					index = 1;
				}
			}
			else {
				if (p.z > center.z) {
					index = 2;
				}
				else {
					index = 3;
				}
			}
		}
		else {
			if (p.y > center.y) {
				if (p.z > center.z) {
					index = 4;
				}
				else {
					index = 5;
				}
			}
			else {
				if (p.z > center.z) {
					index = 6;
				}
				else {
					index = 7;
				}
			}
		}

		// allocate a block
		//          
		//    +---+---+
		//    ! 5 ! 1 !\
		//    +---+---+ \
		//    ! 7 ! 3 +  \
		//    +---+---+   \
		//     \   +---+---+
		//      \  ! 4 ! 0 !
		//       \ +---+---+
		//        \! 6 ! 2 +
		//         +---+---+
		glm::vec3 pmin, pmax;
		Voxel* voxel = nullptr;
		if (m_voxels[current].m_blocks[index] == -1) {
			switch (index) {
			case 0:
				pmin = m_voxels[current].m_aabb.m_center;
				pmax = m_voxels[current].m_aabb.m_p1;
				break;
			case 1:
				pmin = glm::vec3(m_voxels[current].m_aabb.m_center.x, m_voxels[current].m_aabb.m_center.y, m_voxels[current].m_aabb.m_p.z);
				pmax = glm::vec3(m_voxels[current].m_aabb.m_p1.x, m_voxels[current].m_aabb.m_p1.y, m_voxels[current].m_aabb.m_center.z);
				break;
			case 2:
				pmin = glm::vec3(m_voxels[current].m_aabb.m_center.x, m_voxels[current].m_aabb.m_p.y, m_voxels[current].m_aabb.m_center.z);
				pmax = glm::vec3(m_voxels[current].m_aabb.m_p1.x, m_voxels[current].m_aabb.m_center.y, m_voxels[current].m_aabb.m_p1.z);
				break;
			case 3:
				pmin = glm::vec3(m_voxels[current].m_aabb.m_center.x, m_voxels[current].m_aabb.m_p.y, m_voxels[current].m_aabb.m_p.z);
				pmax = glm::vec3(m_voxels[current].m_aabb.m_p1.x, m_voxels[current].m_aabb.m_center.y, m_voxels[current].m_aabb.m_center.z);
				break;
			case 4:
				pmin = glm::vec3(m_voxels[current].m_aabb.m_p.x, m_voxels[current].m_aabb.m_center.y, m_voxels[current].m_aabb.m_center.z);
				pmax = glm::vec3(m_voxels[current].m_aabb.m_center.x, m_voxels[current].m_aabb.m_p1.y, m_voxels[current].m_aabb.m_p1.z);
				break;
			case 5:
				pmin = glm::vec3(m_voxels[current].m_aabb.m_p.x, m_voxels[current].m_aabb.m_center.y, m_voxels[current].m_aabb.m_p.z);
				pmax = glm::vec3(m_voxels[current].m_aabb.m_center.x, m_voxels[current].m_aabb.m_p1.y, m_voxels[current].m_aabb.m_center.z);
				break;
			case 6:
				pmin = glm::vec3(m_voxels[current].m_aabb.m_p.x, m_voxels[current].m_aabb.m_p.y, m_voxels[current].m_aabb.m_center.z);
				pmax = glm::vec3(m_voxels[current].m_aabb.m_center.x, m_voxels[current].m_aabb.m_center.y, m_voxels[current].m_aabb.m_p1.z);
				break;
			case 7:
				pmin = glm::vec3(m_voxels[current].m_aabb.m_p.x, m_voxels[current].m_aabb.m_p.y, m_voxels[current].m_aabb.m_p.z);
				pmax = glm::vec3(m_voxels[current].m_aabb.m_center.x, m_voxels[current].m_aabb.m_center.y, m_voxels[current].m_aabb.m_center.z);
				break;
			}
			child = allocate(pmin, pmax, depth + 1);
			m_voxels[current].m_blocks[index] = child;

			// register the reference down the octree as we created a new node
			m_voxels[current].m_object = nullptr;
		}
		else {
			child = m_voxels[current].m_blocks[index];
		}
		if (!m_voxels[child].m_aabb.inside(p)) {
			__debugbreak();
		}
		current = child;
		depth += 1;
	}

	m_voxels[current].m_object = object;

	// if all 8 subblocks have the same value merge them together upward
	while (history.size() > 0) {
		current = history.top();
		history.pop();

		uint32_t count = 0;
		for (uint32_t i = 0; i < 8; i++) {
			if (m_voxels[current].m_blocks[i] != -1 && m_voxels[m_voxels[current].m_blocks[i]].m_object != nullptr) {
				count++;
			}
		}

		if (count == 8) {
			for (uint32_t i = 0; i < 8; i++) {
				release(m_voxels[current].m_blocks[i]);
			}
			m_voxels[current].m_object = object;
			for (uint32_t i = 0; i < 8; i++) {
				m_voxels[current].m_blocks[i] = -1;
			}
		}
	}
}

#ifdef _DEBUG
void GameEngine::VoxelSpace::debug(void)
{
	size_t free = 0;
	int32_t c = m_freeList;
	while (c > 0) {
		free++;
		c = m_voxels[c].m_nextBlock;
	}

	for (auto& voxel : m_voxels) {
		// only draw end voxels
		if (voxel.m_object != nullptr) {
			g_gaBoundingBoxes.add(&voxel.m_aabb);
		}
	}
}
#endif