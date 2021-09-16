#include "gaBehaviorTree.h"

#include "../gaBehaviorNode.h"

GameEngine::Component::BehaviorTree::BehaviorTree(BehaviorNode* root):
	gaComponent(gaComponent::BehaviorTree),
	m_root(root),
	m_current(root)
{
}

void* GameEngine::Component::BehaviorTree::blackboard(const std::string key)
{
	return m_blackboard[key];
}

void GameEngine::Component::BehaviorTree::blackboard(const std::string key, void* value)
{
	m_blackboard[key] = value;
}

/**
 * let a component deal with a situation
 */
void GameEngine::Component::BehaviorTree::dispatchMessage(gaMessage* message)
{
	if (!m_instanciated) {
		m_root->instanciate(m_entity);
		m_instanciated = true;
		return;
	}

	if (m_current == nullptr) {
		return;
	}

	// execute the current node
	m_current = m_current->dispatchMessage(message);
	if (m_current == nullptr) {
		m_current = m_root;
	}
}
