#include "gaBehaviorNode.h"

#include "gaEntity.h"

GameEngine::BehaviorNode::BehaviorNode(void)
{
}

/**
 *
 */
void GameEngine::BehaviorNode::init(void*)
{
	m_status = Status::RUNNING;
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

/**
 * Add a sub-node to the node
 */
GameEngine::BehaviorNode* GameEngine::BehaviorNode::addNode(BehaviorNode* node)
{
	m_children.push_back(node);
	node->m_parent = this;
	node->m_tree = m_tree;

	if (m_children.size() > 1) {
		m_sequence = true;
	}

	return this;
}

BehaviorNode* GameEngine::BehaviorNode::startChild(int32_t child, void* data)
{
	m_runningChild = child;
	m_children[m_runningChild]->init(data);
	return m_children[m_runningChild]->nextNode();
}

BehaviorNode* GameEngine::BehaviorNode::exitChild(Status s)
{
	m_status = s;

	if (m_parent)
		return m_parent->nextNode();

	return nullptr;
}

/**
 * let a parent take a decision with it's current running child result
 */
GameEngine::BehaviorNode* GameEngine::BehaviorNode::nextNode(void)
{
	if (m_status == Status::RUNNING) {
		return this;
	}

	// by default return a success to move up the tree
	return m_parent->nextNode();
}

BehaviorNode* GameEngine::BehaviorNode::dispatchMessage(gaMessage*message)
{
	return nextNode();
}

void GameEngine::BehaviorNode::sendInternalMessage(int action, const glm::vec3& value)
{
	gaMessage message("_component", "_component", action, value);
	dispatchMessage(&message);
}
