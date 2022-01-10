#include "gaBehaviorNode.h"

#include <imgui.h>
#include <tinyxml2.h>
#include <queue>

#include "../include/imnodes.h"
#include "gaComponent/gaBehaviorTree.h"
#include "gaEntity.h"

static const char* g_className = "Node";

GameEngine::BehaviorNode::BehaviorNode(const char* name):
	m_name(name)
{
	m_className = g_className;
}

/**
 * virtual clone
 */
GameEngine::BehaviorNode* GameEngine::BehaviorNode::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::BehaviorNode* cl;
	if (p) {
		cl = p;
	}
	else {
		cl = new BehaviorNode(m_name);
	}
	cl->m_continueOnError = m_continueOnError;
	cl->m_if_variables = m_if_variables;
	cl->m_if_value = m_if_value;
	return cl;
}

/**
 * recursivlt clone a BT node
 */
BehaviorNode* GameEngine::BehaviorNode::deepClone(void)
{
	GameEngine::BehaviorNode* cl = clone(nullptr);
	GameEngine::BehaviorNode* ch;

	for (auto& child : m_children) {
		ch = child->deepClone();
		cl->m_children.push_back(ch);
		ch->m_parent = cl;
		
	}
	return cl;
}

GameEngine::BehaviorNode::~BehaviorNode(void)
{
	for (auto& child : m_children) {
		delete child;
	}
}

/**
 *
 */
BehaviorNode* GameEngine::BehaviorNode::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::BehaviorNode* node;

	if (used == nullptr) {
		node = new GameEngine::BehaviorNode(name);
	}
	else {
		node = dynamic_cast<GameEngine::BehaviorNode*>(used);
	}

	tinyxml2::XMLElement* xmlOnError = element->FirstChildElement("onerror");
	if (xmlOnError) {
		const char* action = xmlOnError->Attribute("action");
		if (strcmp(action, "succes") == 0) {
			node->m_continueOnError = Status::SUCCESSED;
		}
		else if (strcmp(action, "failed") == 0) {
			node->m_continueOnError = Status::FAILED;
		}
	}

	// Get the exit conditions
	tinyxml2::XMLElement* xmlExit = element->FirstChildElement("exit");
	if (xmlExit != nullptr) {
		tinyxml2::XMLElement* xmlIf = xmlExit->FirstChildElement("if");

		while (xmlIf != nullptr) {
			Variable var;
			Value val;

			if (!var.create(xmlIf)) {
				gaDebugLog(1, "GameEngine::Behavior::Var", "no variable defined for " + (std::string)node->m_name);
				exit(-1);
			}
			if (!val.create(xmlIf)) {
				gaDebugLog(1, "GameEngine::Behavior::Var", "no value defined for " + (std::string)node->m_name);
				exit(-1);
			}

			node->m_if_variables.push_back(var);
			node->m_if_value.push_back(val);

			xmlIf = xmlIf->NextSiblingElement("if");
		}
	}

	return node;
}

//-------------------------------------------------------------

/**
 * check exit conditions
 */
bool GameEngine::BehaviorNode::conditionMet(void)
{
	for (size_t i = 0; i < m_if_variables.size(); i++) {
		if (m_if_variables[i].equal(m_tree, m_if_value[i])) {
			return true;
		}
	}
	return false;
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

//-------------------------------------------------------------------

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

void GameEngine::BehaviorNode::error(Action* r)
{
	switch (m_continueOnError) {
	case Status::SUCCESSED:
		return succeeded(r);
	case Status::FAILED:
		return failed(r);
	default:
		r->action = Status::EXIT;
		r->status = Status::ERR;
		m_status = Status::ERR;
	}
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

	case Status::ERR:
		r->action = BehaviorNode::Status::EXIT;
		if (m_continueOnError != Status::NONE) {
			r->status = m_continueOnError;
		}
		else {
			r->status = Status::ERR;
		}
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
 * Debugger
 */
void GameEngine::BehaviorNode::debugGUIinline(BehaviorNode* current, float x, float& y)
{
	static uint32_t attr = 0;
	static char tmp[64];
	const char* p = m_name;
	bool changeColor = false;

	if (this == current) {
		snprintf(tmp, sizeof(tmp), ">%s", m_name);
		p = tmp;
	}
	
	switch (m_status) {
	case Status::RUNNING:
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(81, 48, 04, 255));
		changeColor = true;
		break;
	case Status::SUCCESSED:
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(61, 150, 04, 255));
		changeColor = true;
		break;
	case Status::FAILED:
		ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(150, 38, 04, 255));
		changeColor = true;
		break;
	}
	ImNodes::BeginNode(m_id);
	ImNodes::BeginNodeTitleBar();

	strncpy_s(tmp, m_className, sizeof(tmp));
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
			snprintf(tmp, sizeof(tmp), "%s", n->m_name);
			strcpy_s(tmp + 24, sizeof(tmp), "...");
			const float label_width = ImGui::CalcTextSize(tmp).x;
			ImGui::Indent(200.0f - label_width);
			ImGui::TextUnformatted(tmp);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 15.0f);
				ImGui::TextUnformatted(n->m_name);
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
		ImNodes::EndOutputAttribute();
		i++;
	}
	ImNodes::EndNode();
	if (changeColor) {
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

const char* GameEngine::BehaviorNode::Action::debug(void)
{
	static std::map <GameEngine::BehaviorNode::Status, const char*> m_debugActions = {
	{BehaviorNode::Status::START_CHILD, "Start Child"},
	{BehaviorNode::Status::EXIT, "Exit"},
	{BehaviorNode::Status::EXECUTE, "Execute"},
	{BehaviorNode::Status::RUNNING, "Running"}
	};

	if (m_debugActions.count(action) > 0) {
		return m_debugActions[action];
	}

	return "unknown";
}
