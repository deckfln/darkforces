#include "gaOctree.h"

GameEngine::Octree::Octree(int32_t extend, uint32_t depth):
	m_depth(depth)
{
	node root;
	glm::vec3 p0(-extend, -extend, -extend);
	glm::vec3 p1(extend, extend, extend);
	root.m_aabb.set(p0, p1);

	m_quadrant.push_back(root);
}

/**
 *
 */
void GameEngine::Octree::mqueue(
	uint32_t depth,
	fwAABBox& object, 
	fwAABBox& subtree, 
	glm::vec3& pmin, 
	glm::vec3& pmax, 
	uint32_t currentNode, 
	uint32_t i)
{
	subtree.set(pmin, pmax);
	if (object.intersect(subtree)) {
		uint32_t size;

		if (m_quadrant[currentNode].m_blocks[i] == -1) {
			node nroot;
			nroot.m_aabb = subtree;

			size = m_quadrant.size();

			m_quadrant.push_back(nroot);

			m_quadrant[currentNode].m_blocks[i] = size;
		}
		else {
			size = m_quadrant[currentNode].m_blocks[i];
		}
		m_queue.push(quadrant(size, subtree, depth));
	}
}

/**
 * merge a block
 */
void GameEngine::Octree::merge(int32_t current)
{
	std::vector<void*> objects = m_quadrant[m_quadrant[current].m_blocks[0]].m_objects;
	for (uint32_t i = 0; i < 8; i++) {
		release(m_quadrant[current].m_blocks[i]);
	}
	m_quadrant[current].m_objects = objects;
	for (uint32_t i = 0; i < 8; i++) {
		m_quadrant[current].m_blocks[i] = -1;
	}

	m_quadrant[current].m_leaf = 1;
}

/**
 *
 */
void GameEngine::Octree::release(int32_t index)
{
	m_freeList.push(index);
	m_quadrant[index].m_leaf = 2;
}

/**
 *
 */
void GameEngine::Octree::add(fwAABBox& aabb, void* object)
{
	int32_t current = 0;
	int32_t child = -1;
	std::stack<int32_t> history;
	uint32_t depth = 0;
	fwAABBox tree;
	fwAABBox subtree;
	glm::vec3 pmin, pmax;

	m_queue.push(
		quadrant(0, m_quadrant[0].m_aabb, 0)
	);

	while (m_queue.size() > 0) {
		quadrant& pop = m_queue.top();
		m_queue.pop();

		current = pop.index;
		depth = pop.depth;

		if (depth == m_depth) {
			// bottom of the octree
			m_quadrant[pop.index].m_objects.push_back(object);
			m_quadrant[pop.index].m_leaf = 1;
		}
		else {
			fwAABBox cAABB = m_quadrant[current].m_aabb;

			// check against sub-quadrant 0,0
			pmin = cAABB.m_center;
			pmax = cAABB.m_p1;
			mqueue(depth + 1, aabb, subtree, pmin, pmax, current, 0);

			pmin = glm::vec3(cAABB.m_center.x, cAABB.m_center.y, cAABB.m_p.z);
			pmax = glm::vec3(cAABB.m_p1.x, cAABB.m_p1.y, cAABB.m_center.z);
			mqueue(depth + 1, aabb, subtree, pmin, pmax, current, 1);

			pmin = glm::vec3(cAABB.m_center.x, cAABB.m_p.y, cAABB.m_center.z);
			pmax = glm::vec3(cAABB.m_p1.x, cAABB.m_center.y, cAABB.m_p1.z);
			mqueue(depth + 1, aabb, subtree, pmin, pmax, current, 2);

			pmin = glm::vec3(cAABB.m_center.x, cAABB.m_p.y, cAABB.m_p.z);
			pmax = glm::vec3(cAABB.m_p1.x, cAABB.m_center.y, cAABB.m_center.z);
			mqueue(depth + 1, aabb, subtree, pmin, pmax, current, 3);

			pmin = glm::vec3(cAABB.m_p.x, cAABB.m_center.y, cAABB.m_center.z);
			pmax = glm::vec3(cAABB.m_center.x, cAABB.m_p1.y, cAABB.m_p1.z);
			mqueue(depth + 1, aabb, subtree, pmin, pmax, current, 4);

			pmin = glm::vec3(cAABB.m_p.x, cAABB.m_center.y, cAABB.m_p.z);
			pmax = glm::vec3(cAABB.m_center.x, cAABB.m_p1.y, cAABB.m_center.z);
			mqueue(depth + 1, aabb, subtree, pmin, pmax, current, 5);

			pmin = glm::vec3(cAABB.m_p.x, cAABB.m_p.y, cAABB.m_center.z);
			pmax = glm::vec3(cAABB.m_center.x, cAABB.m_center.y, cAABB.m_p1.z);
			mqueue(depth + 1, aabb, subtree, pmin, pmax, current, 6);

			pmin = glm::vec3(cAABB.m_p.x, cAABB.m_p.y, cAABB.m_p.z);
			pmax = glm::vec3(cAABB.m_center.x, cAABB.m_center.y, cAABB.m_center.z);
			mqueue(depth + 1, aabb, subtree, pmin, pmax, current, 7);
		}
	}
}

/**
 *
 */
void GameEngine::Octree::compress(void)
{
	std::stack<uint32_t> down;
	std::stack<uint32_t> up;
	uint32_t current;
	uint32_t count = 0;

	down.push(0);

	while (down.size() > 0) {
		current = down.top();
		down.pop();
		node& node = m_quadrant[current];

		for (uint32_t i = 0; i < 8; i++) {
			if (node.m_blocks[i] != -1) {
				if (m_quadrant[node.m_blocks[i]].m_objects.size() == 0) {
					down.push(node.m_blocks[i]);
					up.push(node.m_blocks[i]);
				}
			}
		}
	}

	while (up.size() > 0) {
		current = up.top();
		up.pop();
		node& node = m_quadrant[current];

		// if all 8 subblocks have the same value merge them together upward
		uint32_t count = 0;
		bool found = false;
		std::vector<void*> value;

		if (node.m_blocks[0] != -1) {
			value = m_quadrant[node.m_blocks[0]].m_objects;
			if (value.size() != 0) {
				// don't try to merge subnodes
				// only leaves
				for (uint32_t i = 0; i < 8; i++) {
					if (node.m_blocks[i] == -1) {
						// if one child is empty, the 8 blocks cannot be identical
						break;
					}
					if (m_quadrant[node.m_blocks[i]].m_objects == value) {
						count++;
					}
				}
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
const std::vector<void*>& GameEngine::Octree::find(const glm::vec3& p)
{
	int32_t current = 0;
	uint32_t depth = 0;
	uint32_t index;

	while (current >= 0 && depth <= m_depth) {
		node& cNode = m_quadrant[current];
		const glm::vec3& center = cNode.m_aabb.m_center;

		// reached a leaf
		if (cNode.m_objects.size() > 0) {
			return cNode.m_objects;
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

		current = cNode.m_blocks[index];
	}

	static std::vector<void*> empty;
	return empty;
}

/**
 *
 */
#ifdef _DEBUG

#include "../darkforces/dfSector.h"

void GameEngine::Octree::debug(void)
{
	std::stack<uint32_t> list;
	uint32_t current;
	uint32_t count = 0;

	list.push(0);

	while (list.size() > 0) {
		current = list.top();
		list.pop();
		node& node = m_quadrant[current];

		if (node.m_objects.size() > 0) {
			/*
			dfSector* sector = (dfSector*)voxel.m_object;
			voxel.m_aabb.color(glm::vec3(sector->m_ambient / 32.0f, sector->m_ambient / 32.0f, sector->m_ambient / 32.0f));
			*/
			g_gaBoundingBoxes.add(&node.m_aabb);
		}
		else {
			for (uint32_t i = 0; i < 8; i++) {
				if (node.m_blocks[i] != -1) {
					list.push(node.m_blocks[i]);
				}
			}
		}
	}
}
#endif
