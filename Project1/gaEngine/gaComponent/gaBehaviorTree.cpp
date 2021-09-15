#include "gaBehaviorTree.h"

#include "../gaBehaviorNode.h"

GameEngine::Component::BehaviorTree::BehaviorTree(BehaviorNode* root):
	gaComponent(gaComponent::BehaviorTree),
	m_root(root),
	m_current(root)
{
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
	m_current->dispatchMessage(message);

	if (m_current->status() != GameEngine::BehaviorNode::Status::RUNNING) {

		// the root node is the minimum active node
		if (m_current != m_root) {
			// go back to parent
			m_current = m_current->parent();

			if (m_current) {
				// get the node to pick the next one
				m_current = m_current->nextNode();
			}
		}
	}
}
