#include "gaBehaviorTree.h"

#include <imgui.h>

#include "../gaEntity.h"
#include "../gaBehaviorNode.h"
#include "../flightRecorder/frBehaviorTree.h"

static const char* g_className = "BehaviorTree";

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
 */void GameEngine::Component::BehaviorTree::dispatchMessage(gaMessage* message)
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

/**
 * display the component in the debugger
 */
void GameEngine::Component::BehaviorTree::debugGUIinline(void)
{
	if (ImGui::TreeNode(g_className)) {
		m_root->debugGUIinline(m_current);
		ImGui::TreePop();
	}
}

/**
 * size of the component record
 */
inline uint32_t GameEngine::Component::BehaviorTree::recordSize(void)
{
	return sizeof(GameEngine::flightRecorder::BehaviorTree);
}

/**
 * save the component state in a record
 */
uint32_t GameEngine::Component::BehaviorTree::recordState(void* record)
{
	GameEngine::flightRecorder::BehaviorTree* r = static_cast<GameEngine::flightRecorder::BehaviorTree*>(record);

	if (m_nbnodes == 0) {
		// lazy count off nodes in the tree
		m_nbnodes = m_root->count(m_nbnodes);

		m_nodes.resize(m_nbnodes);
		m_root->record(m_nodes);
	}

	uint32_t len = sizeof(GameEngine::flightRecorder::BehaviorTree);
	r->size = len;
	r->nbNodes = m_nbnodes;
	r->m_current = m_current->id();

	// record the nodes
	char *p = &r->nodes[0];
	uint32_t s = sizeof(r->nodes);
	uint32_t l;

	for (auto node : m_nodes) {
		l = node->recordState(p);
		p += l;
		s -= l;
		len += l;

		if (s <= 0) {
			__debugbreak();
		}
	}

	uint32_t l1 = p - (char *)r;
	r->size = l1;

	return r->size;
}

/**
 * reload a component state from a record
 */
uint32_t GameEngine::Component::BehaviorTree::loadState(void* record)
{
	GameEngine::flightRecorder::BehaviorTree* r = static_cast<GameEngine::flightRecorder::BehaviorTree*>(record);

	BehaviorNode* current = m_root->find(r->m_current);
	if (current) {
		m_current = current;
	}
	else {
		m_current = m_root;
	}

	// load the nodes
	char *p = &r->nodes[0];
	uint32_t i;
	uint32_t l;

	for (i = 0; i < m_nbnodes; i++) {
		l = m_nodes[i]->loadState(p);
		p += l;
	}

	return r->size;
}
