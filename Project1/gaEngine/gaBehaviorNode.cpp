#include "gaBehaviorNode.h"

#include <imgui.h>
#include <tinyxml2.h>
#include <queue>

#include "../include/imnodes.h"
#include "gaComponent/gaBehaviorTree.h"
#include "gaEntity.h"

GameEngine::BehaviorNode::BehaviorNode(const char* name):
	m_name(name)
{
}

/**
 * bind all nodes to the tree
 */
void GameEngine::BehaviorNode::tree(Component::BehaviorTree* tree)
{
	std::queue<BehaviorNode*> nodes;
	BehaviorNode* node;

	nodes.push(this);

	while (!nodes.empty()) {
		node = nodes.front();
		nodes.pop();

		node->m_tree = tree;
		for (auto child : node->m_children) {
			nodes.push(child);
		}
	}
}

/**
 *
 */
void GameEngine::BehaviorNode::init(void* data)
{
	m_status = Status::RUNNING;
	m_runningChild = -1;
	m_data = data;

	// reset the status of all children
	for (auto& child : m_children) {
		child->status(Status::NONE);
	}
}

/**
 *
 */
BehaviorNode* GameEngine::BehaviorNode::create(const char* name, tinyxml2::XMLElement* element)
{
	return new BehaviorNode(name);
}

/**
 * bind node to an entity
 */
void GameEngine::BehaviorNode::instanciate(gaEntity* entity)
{
	m_id = m_tree->lastNode();
	m_entryAttr = m_tree->lastAttrId();

	m_entity = entity;

	for (auto child : m_children) {
		m_childrenID.push_back(m_tree->lastAttrId());
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

//*********************************************************

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

//*********************************************************

/**
 *
 */
void GameEngine::BehaviorNode::dispatchMessage(gaMessage*message, BehaviorNode::Action* r)
{
	r->action = BehaviorNode::Status::RUNNING;
}

//*********************************************************

/**
 *
 */
void GameEngine::BehaviorNode::debugGUIinline(BehaviorNode* current, float x, float& y)
{
	static uint32_t attr = 0;
	static char tmp[64];
	const char* p = m_name;

	if (this == current) {
		snprintf(tmp, sizeof(tmp), ">%s", m_name);
		p = tmp;
	}
	
	if (this == current) {
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(81, 48, 04, 255));
	}
	ImNodes::BeginNode(m_id);
	ImNodes::BeginNodeTitleBar();

	strncpy_s(tmp, m_name, sizeof(tmp));
	float lx = ImGui::CalcTextSize(tmp).x;
	if (lx > 200) {
		uint32_t l = strlen(tmp) / 2;
		for (char* c = tmp + l; *c != 0 ; c++) {
			if (*c == ' ') {
				*c = '\n';
				break;
			}
		}
	}
	ImGui::TextUnformatted(tmp);
	ImNodes::EndNodeTitleBar();

	ImNodes::BeginInputAttribute(m_entryAttr);
	debugGUInode();
	ImNodes::EndInputAttribute();

	uint32_t i=0;
	for (auto n : m_children) {
		ImNodes::BeginOutputAttribute(m_childrenID[i]);

		if (m_children.size() > 1) {
			snprintf(tmp, sizeof(tmp), "%d", i);
			const float label_width = ImGui::CalcTextSize(tmp).x;
			ImGui::Indent(200.0f - label_width);
			ImGui::TextUnformatted(tmp);
		}
		ImNodes::EndOutputAttribute();
		i++;
	}
	ImNodes::EndNode();
	if (this == current) {
		ImNodes::PopColorStyle();
	}

	if (!m_pined) {
		m_pined = true;
		ImNodes::SetNodeGridSpacePos(m_id, ImVec2(x, y));
	}

	if (m_children.size() == 0) {
		y += 100.0f; // next line
	}

	i = 0;
	for (auto n : m_children) {
		m_children[i]->debugGUIinline(current, x + 300, y);
		ImNodes::Link(m_childrenID[i], m_childrenID[i], m_children[i]->m_entryAttr);
		i++;
	}
}

/**
 *
 */
void GameEngine::BehaviorNode::debugGUInode(void)
{
}

//*********************************************************

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

