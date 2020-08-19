#include "AABBoxTree.h"

using namespace GameEngine;

AABBoxTree::AABBoxTree():
	fwAABBox()
{
}

AABBoxTree::AABBoxTree(const glm::vec3& p, const glm::vec3& p1):
	fwAABBox(p, p1)
{
}

/**
 * Add a child to the current AABB and extend it
 */
AABBoxTree& AABBoxTree::add(AABBoxTree*child)
{
	m_children.push_back(child);
	extend(*child);

	return *this;
}

/**
 * find the smallest AABB containing the given box
 */
bool AABBoxTree::find(const fwAABBox& box, std::vector<AABBoxTree*>& results)
{
	// if we are at the bottom
	if (m_children.size() == 0) {
		if (intersect(box)) {
			results.push_back(this);
			return true;
		}
		return false;
	}

	// otherwise test the children
	// if the parent intersect with the box, look for a smallest intersection
	bool r = false;
	if (intersect(box)) {
		for (auto& child : m_children) {
			if (child->find(box, results)) {
				r = true;
			}
		}
	}

	return r;
}