#include "gaBehaviorTree.h"

#include <imgui.h>
#include <deque>

#include "../gaDebug.h"
#include "../gaEntity.h"
#include "../gaBehaviorNode.h"
#include "../gaBehavior.h"
#include "../gaBehaviorNode/gaBSounds.h"
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

GameEngine::Component::BehaviorTree::~BehaviorTree(void)
{
	delete m_root;
}

/**
 * create a tree from XML data
 */
void GameEngine::Component::BehaviorTree::parse(const std::string& data, const std::map<std::string, std::string>& includes)
{
	m_root = GameEngine::Behavior::loadTree(data, includes, this);
	m_current = m_root;

	m_root->tree(this);
	m_root->init(nullptr);

}

//---------------------------------------------

/**
 * manager message handlers
 */
void GameEngine::Component::BehaviorTree::handlers(uint32_t message, msgHandler handler)
{
	m_handlers[message] = handler;
}

/**
 * manage plugins handling messages
 */
void GameEngine::Component::BehaviorTree::plugins(GameEngine::Behavior::Plugin::Base::pluginHandler plugin)
{
	m_plugins.push_back((*plugin)());
}

//---------------------------------------------

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

	if (m_entity->name() != "OFFCFIN.WAX(21)" && !m_active) {
		// the AI is deactivated for the entity
		return;
	}

	m_debug = true;

	// pass the message to all plugins
	for (auto plugin : m_plugins) {
		plugin->dispatchMessage(m_blackboard, message);
	}

	// use general purpose message handlers
	if (m_handlers.count(message->m_action)) {
		(this->*m_handlers[message->m_action])(message);
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

#ifdef _DEBUG
			if (m_debug) {
				/*
					gaDebugLog(1, "GameEngine::BehaviorTree",
					std::to_string(message->m_frame) + " " +
					m_entity->name() + " start  " + m_current->name()
				);
				*/
				m_current->debugConsoleIn(this);
			}
			m_depth++;
#endif

			r.action = BehaviorNode::Status::EXECUTE;
			break;

		case BehaviorNode::Status::EXIT:
			m_depth--;
#ifdef _DEBUG
			if (m_debug) {
				/*
				gaDebugLog(1, "GameEngine::BehaviorTree",
					std::to_string(message->m_frame) + " " +
					m_entity->name() + " exit " + m_current->name() + " with status " + std::to_string((int)r.status)
				);
				*/
				m_current->debugConsoleOut(this);
			}
#endif
			if (m_current->m_parent) {
				// move to parent node
				m_current = m_current->m_parent;
				m_current->activated();
				r.action = BehaviorNode::Status::EXECUTE;
			}
			else {
				// when exiting the root, reinit the tree,
				r.action = BehaviorNode::Status::RUNNING;
				m_current->init(r.data);
			}
			break;

		case BehaviorNode::Status::EXECUTE:
			count++;
#ifdef _DEBUG
			/*
			if (m_debug) {
				gaDebugLog(1, "GameEngine::BehaviorTree",
					std::to_string(message->m_frame) + " " +
					m_entity->name() + " execute " + m_current->name()
				);
			}
			*/
#endif
			m_current->execute(&r);
			break;
		}
		if (count > 50) {
			__debugbreak();
		}
	}

	if (m_current == nullptr) {
		m_current = m_root;
	}
}

//---------------------------------------------

#ifdef _DEBUG
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
		ImGui::TreePop();

		if (m_context == nullptr) {
			m_context = ImNodes::EditorContextCreate();
		}

		ImNodes::EditorContextSet(m_context);
		ImGui::Begin(m_entity->name().c_str());

		ImNodes::BeginNodeEditor();
		float y = 0;
		m_root->debugGUIinline(this, m_current, 0, y);
		ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomLeft);
		ImNodes::EndNodeEditor();

		ImGui::End();

		m_debug = true;
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
		m_debug = false;
	}
}
#endif
//---------------------------------------------

/**
 * size of the component record
 */
inline uint32_t GameEngine::Component::BehaviorTree::recordSize(void)
{
	return sizeof(GameEngine::flightRecorder::BehaviorTree) + m_blackboard.recordSize();
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
	uint32_t lBlack = m_blackboard.recordState(p);

	r->size = l1 + lBlack;

	return r->size;
}

/**
 * reload a component state from a record
 */
uint32_t GameEngine::Component::BehaviorTree::loadState(void* record)
{
	/*
	if (m_entity->name() == "OFFCFIN.WAX(21)") {
		__debugbreak();
	}
	*/
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

	uint32_t lBlack = m_blackboard.loadState(p);
	p += lBlack;  

	return r->size;
}
