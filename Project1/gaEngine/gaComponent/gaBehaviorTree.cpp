#include "gaBehaviorTree.h"

#include <imgui.h>

#include "../gaEntity.h"
#include "../gaBehaviorNode.h"
#include "../gaBehavior.h"
#include "../flightRecorder/frBehaviorTree.h"

static const char* g_className = "BehaviorTree";

uint32_t GameEngine::Component::BehaviorTree::m_lastId = 0;
uint32_t GameEngine::Component::BehaviorTree::m_lastNode = 0;

GameEngine::Component::BehaviorTree::BehaviorTree(void):
	gaComponent(gaComponent::BehaviorTree)
{
}

GameEngine::Component::BehaviorTree::BehaviorTree(BehaviorNode* root):
	gaComponent(gaComponent::BehaviorTree),
	m_root(root),
	m_current(root)
{
}

/**
 * create a tree from XML data
 */
void GameEngine::Component::BehaviorTree::parse(const std::string& data)
{
	m_root = GameEngine::Behavior::loadTree(data);
	m_current = m_root;

	m_root->tree(this);
	m_root->init(nullptr);

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

	// pass the message to the current node
	BehaviorNode::Action r;
	m_current->dispatchMessage(message, &r);

	uint32_t count = 0;	// security counter

	// Navigate the tree
	while (r.action != BehaviorNode::Status::RUNNING) {
		switch (r.action) {
		case BehaviorNode::Status::START_CHILD:
			m_current->m_runningChild = r.child;
			m_current = m_current->m_children[m_current->m_runningChild];
			m_current->init(r.data);

			r.action = BehaviorNode::Status::EXECUTE;
			break;

		case BehaviorNode::Status::EXIT:
			if (m_current->m_parent) {
				m_current = m_current->m_parent;
				m_current->activated();
				r.action = BehaviorNode::Status::EXECUTE;
			}
			else {
				r.action = BehaviorNode::Status::RUNNING;
			}
			break;;

		case BehaviorNode::Status::EXECUTE:
			m_current->execute(&r);
			break;
		}
		if (count++ > 50) {
			__debugbreak();
		}
	}

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
		/*
			const ImVec2 p = ImGui::GetCursorScreenPos();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			const ImU32 col32 = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
			draw_list->AddCircle(ImVec2(p.x+4, p.y+4), 8.0, col32, 6, 2.0);
			draw_list->AddRect(ImVec2(p.x, p.y), ImVec2(p.x + 100, p.y + 40), col32);
			ImGui::Dummy(ImVec2(100.0, 40.0));
		*/
		if (m_context == nullptr) {
			m_context = ImNodes::EditorContextCreate();
		}

		ImNodes::EditorContextSet(m_context);
		ImGui::Begin(m_entity->name().c_str());

		ImNodes::BeginNodeEditor();
		float y = 0;
		m_root->debugGUIinline(m_current, 0, y);
		ImNodes::EndNodeEditor();

		ImGui::End();

		ImGui::TreePop();
	}
	else {
		if (m_context != nullptr) {
			// destroy the whole editor
			ImNodes::EditorContextFree(m_context);
			m_context = nullptr;

			for (auto node : m_nodes) {
				node->m_pined = false;
			}
		}

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
