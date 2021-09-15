#include "gaBehaviorNode.h"

#include "gaEntity.h"

GameEngine::BehaviorNode::BehaviorNode(void)
{
}

/**
 * bind node to an entity
 */
void GameEngine::BehaviorNode::instanciate(gaEntity* entity)
{
	m_entity = entity;

	for (auto child : m_children) {
		child->instanciate(entity);
	}
}

GameEngine::BehaviorNode* GameEngine::BehaviorNode::addNode(BehaviorNode* node)
{
	m_children.push_back(node);

	return this;
}

/**
 * let a parent take a decision with it's current running child result
 */
GameEngine::BehaviorNode* GameEngine::BehaviorNode::nextNode(void)
{
	// by default return a success to move up the tree
	return m_parent;
}

void GameEngine::BehaviorNode::dispatchMessage(gaMessage*message)
{
}