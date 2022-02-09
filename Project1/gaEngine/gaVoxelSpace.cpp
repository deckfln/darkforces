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
		m_voxels[voxel].m_nextBlock = -1;
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
void GameEngine::VoxelSpace::split(uint32_t current)
{
	glm::vec3 pmin, pmax;

	GameEngine::Voxel& voxel = m_voxels[current];

	pmin = voxel.m_aabb.m_center;
	pmax = voxel.m_aabb.m_p1;
	voxel.m_blocks[0] = allocate(pmin, pmax, voxel.m_level + 1);
	m_voxels[voxel.m_blocks[0]].m_object = voxel.m_object;

	pmin = glm::vec3(voxel.m_aabb.m_center.x, voxel.m_aabb.m_center.y, voxel.m_aabb.m_p.z);
	pmax = glm::vec3(voxel.m_aabb.m_p1.x, voxel.m_aabb.m_p1.y, voxel.m_aabb.m_center.z);
	voxel.m_blocks[1] = allocate(pmin, pmax, voxel.m_level + 1);
	m_voxels[voxel.m_blocks[1]].m_object = voxel.m_object;

	pmin = glm::vec3(voxel.m_aabb.m_center.x, voxel.m_aabb.m_p.y, voxel.m_aabb.m_center.z);
	pmax = glm::vec3(voxel.m_aabb.m_p1.x, voxel.m_aabb.m_center.y, voxel.m_aabb.m_p1.z);
	voxel.m_blocks[2] = allocate(pmin, pmax, voxel.m_level + 1);
	m_voxels[voxel.m_blocks[2]].m_object = voxel.m_object;

	pmin = glm::vec3(voxel.m_aabb.m_center.x, voxel.m_aabb.m_p.y, voxel.m_aabb.m_p.z);
	pmax = glm::vec3(voxel.m_aabb.m_p1.x, voxel.m_aabb.m_center.y, voxel.m_aabb.m_center.z);
	voxel.m_blocks[3] = allocate(pmin, pmax, voxel.m_level + 1);
	m_voxels[voxel.m_blocks[3]].m_object = voxel.m_object;

	pmin = glm::vec3(voxel.m_aabb.m_p.x, voxel.m_aabb.m_center.y, voxel.m_aabb.m_center.z);
	pmax = glm::vec3(voxel.m_aabb.m_center.x, voxel.m_aabb.m_p1.y, voxel.m_aabb.m_p1.z);
	voxel.m_blocks[4] = allocate(pmin, pmax, voxel.m_level + 1);
	m_voxels[voxel.m_blocks[4]].m_object = voxel.m_object;

	pmin = glm::vec3(voxel.m_aabb.m_p.x, voxel.m_aabb.m_center.y, voxel.m_aabb.m_p.z);
	pmax = glm::vec3(voxel.m_aabb.m_center.x, voxel.m_aabb.m_p1.y, voxel.m_aabb.m_center.z);
	voxel.m_blocks[5] = allocate(pmin, pmax, voxel.m_level + 1);
	m_voxels[voxel.m_blocks[5]].m_object = voxel.m_object;

	pmin = glm::vec3(voxel.m_aabb.m_p.x, voxel.m_aabb.m_p.y, voxel.m_aabb.m_center.z);
	pmax = glm::vec3(voxel.m_aabb.m_center.x, voxel.m_aabb.m_center.y, voxel.m_aabb.m_p1.z);
	voxel.m_blocks[6] = allocate(pmin, pmax, voxel.m_level + 1);
	m_voxels[voxel.m_blocks[6]].m_object = voxel.m_object;

	pmin = glm::vec3(voxel.m_aabb.m_p.x, voxel.m_aabb.m_p.y, voxel.m_aabb.m_p.z);
	pmax = glm::vec3(voxel.m_aabb.m_center.x, voxel.m_aabb.m_center.y, voxel.m_aabb.m_center.z);
	voxel.m_blocks[7] = allocate(pmin, pmax, voxel.m_level + 1);
	m_voxels[voxel.m_blocks[7]].m_object = voxel.m_object;

	voxel.m_object = nullptr;
}

/**
 *
 */
void GameEngine::VoxelSpace::merge(int32_t voxel)
{
	void *object = m_voxels[m_voxels[voxel].m_blocks[0]].m_object;
	for (uint32_t i = 0; i < 8; i++) {
		release(m_voxels[voxel].m_blocks[i]);
	}
	m_voxels[voxel].m_object = object;
	for (uint32_t i = 0; i < 8; i++) {
		m_voxels[voxel].m_blocks[i] = -1;
	}
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

/**
 *
 */
void GameEngine::VoxelSpace::add(const glm::vec3& p, void* object)
{
	int32_t current = 0;
	int32_t child = -1;
	std::stack<int32_t> history;
	uint32_t depth = 0;

	while (depth < m_depth) {
		GameEngine::Voxel& pvoxel = m_voxels[current];

		history.push(current);

		// if the current voxel is a merge of lower voxels, drop out if the value to store is the same as the value below
		if (m_voxels[current].m_object != nullptr) {
			if (m_voxels[current].m_object == object) {
				break;
			}
			else {
				// want add a value that is not the merged one, will need to split the lower voxels
				split(current);
			}
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
		/*
		if (!m_voxels[child].m_aabb.inside(p)) {
			__debugbreak();
		}
		*/
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
			merge(current);
		}
	}
}

/**
 *
 */
void* GameEngine::VoxelSpace::find(const glm::vec3& p)
{
	int32_t current = 0;
	uint32_t depth = 0;
	uint32_t index;

	while (current >= 0 && depth <= m_depth) {
		GameEngine::Voxel& voxel = m_voxels[current];
		/*
		if (!voxel.m_aabb.inside(p)) {
			__debugbreak();
		}
		voxel.m_aabb.color(glm::vec3(1.0, 0.0, 0.0));
		*/
		const glm::vec3& center = voxel.m_aabb.m_center;

		// reached a leaf
		if (voxel.m_object != nullptr) {
			return voxel.m_object;
		}

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

		// drill down
		depth++;
		
		current = voxel.m_blocks[index];
	}

	return nullptr;
}

/**
 *
 */
#ifdef _DEBUG

#include "../darkforces/dfSector.h"

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
			dfSector* sector = (dfSector*)voxel.m_object;
			voxel.m_aabb.color(glm::vec3(sector->m_ambient/32.0f, sector->m_ambient / 32.0f, sector->m_ambient / 32.0f));
			g_gaBoundingBoxes.add(&voxel.m_aabb);
		}
	}
}
#endif