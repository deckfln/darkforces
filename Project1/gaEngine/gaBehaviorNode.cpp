#include "gaBehaviorNode.h"

#include <imgui.h>

#include "gaEntity.h"

GameEngine::BehaviorNode::BehaviorNode(const char* name):
	m_name(name)
{
}

/**
 *
 */
void GameEngine::BehaviorNode::init(void* data)
{
	m_status = Status::RUNNING;
	m_runningChild = -1;
	m_data = data;
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

/**
 * count nodes in a tree a register ID
 */
uint32_t GameEngine::BehaviorNode::count(uint32_t count)
{
	m_id = count++;

	for (auto node : m_children) {
		count = node->count(count);
	}
	return count;
}

/**
 * find the node with ID
 */
BehaviorNode* GameEngine::BehaviorNode::find(uint32_t id)
{
	if (m_id == id) {
		return this;
	}

	for (auto node : m_children) {
		BehaviorNode* r = node->find(id);
		if (r) {
			return r;
		}
	}

	return nullptr;
}

/**
 * record the node in a list
 */
void GameEngine::BehaviorNode::record(std::vector<BehaviorNode*>& nodes)
{
	nodes[m_id] = this;

	for (auto node : m_children) {
		node->record(nodes);
	}
}

/**
 * Node failed
 */
void GameEngine::BehaviorNode::failed(Action* r)
{
	r->action = Status::EXIT;
	r->status = Status::FAILED;
	m_status = Status::FAILED;
}

void GameEngine::BehaviorNode::succeeded(Action* r)
{
	r->action = Status::EXIT;
	r->status = Status::SUCCESSED;
	m_status = Status::SUCCESSED;
}

void GameEngine::BehaviorNode::startChild(Action* r, uint32_t child, void* data)
{
	r->action = Status::START_CHILD;
	r->child = child;
	r->data = data;
}

/**
 * a node get re-activated after a child exit
 */
void GameEngine::BehaviorNode::activated(void)
{
}

/**
 * let a parent take a decision with it's current running child result
 */
void GameEngine::BehaviorNode::execute(BehaviorNode::Action *r)
{
	switch (m_status) {
	case Status::RUNNING:
		r->action = BehaviorNode::Status::RUNNING;
		break;

	case Status::FAILED:
	case Status::SUCCESSED:
		r->action = BehaviorNode::Status::EXIT;
		r->status = m_status;
		break;

	default:
		__debugbreak();
	}
}

/**
 *
 */
void GameEngine::BehaviorNode::dispatchMessage(gaMessage*message, BehaviorNode::Action* r)
{
	r->action = BehaviorNode::Status::RUNNING;
}

/**
 *
 */
void GameEngine::BehaviorNode::debugGUIinline(BehaviorNode* current)
{
	static char tmp[64];
	const char* p = m_name;

	if (this == current) {
		snprintf(tmp, sizeof(tmp), ">%s", m_name);
		p = tmp;
	}

	if (ImGui::TreeNode(p)) {
		for (auto n: m_children)
			n->debugGUIinline(current);
		ImGui::TreePop();
	}
}

/**
 * flight recorder
 */
uint32_t GameEngine::BehaviorNode::recordState(void* record)
{
	FlightRecorder::BehaviorNode* r = static_cast<FlightRecorder::BehaviorNode*>(record);

	r->size = sizeof(FlightRecorder::BehaviorNode);
	r->id = m_id;
	r->status = static_cast<uint32_t>(m_status);
	r->runningChild = m_runningChild;

	return r->size;
}

uint32_t GameEngine::BehaviorNode::loadState(void* record)
{
	FlightRecorder::BehaviorNode* r = static_cast<FlightRecorder::BehaviorNode*>(record);

	m_status = static_cast<Status>(r->status);
	m_runningChild = r->runningChild;

	return r->size;
}

