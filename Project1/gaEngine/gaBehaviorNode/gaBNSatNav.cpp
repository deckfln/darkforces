#include "gaBNSatNav.h"

#include <map>
#include <imgui.h>
#include <tinyxml2.h>

#include "../gaEntity.h"
#include "../gaNavMesh.h"
#include "../World.h"
#include "../gaComponent/gaBehaviorTree.h"

#include "../flightRecorder/frPathFinding.h"

GameEngine::Behavior::SatNav::SatNav(void) :
	MoveTo("SatNave")
{
}

/**
 * create the component
 */
GameEngine::Behavior::SatNav::SatNav(const char *name) :
	MoveTo(name)
{
}

GameEngine::Behavior::SatNav::SatNav(const char *name, float speed) :
	MoveTo(name, speed)
{
}

/**
 * create through clone
 */
GameEngine::BehaviorNode* GameEngine::Behavior::SatNav::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::SatNav* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::SatNav*>(p);
	}
	else {
		cl = new GameEngine::Behavior::SatNav(m_name);
	}
	cl->m_move2variable = m_move2variable;
	cl->m_destName = m_destName;
	cl->m_variable_type = m_variable_type;
	GameEngine::Behavior::MoveTo::clone(cl);
	return cl;
}

/**
 * create from XML
 */
GameEngine::BehaviorNode* GameEngine::Behavior::SatNav::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::SatNav* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::SatNav(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::SatNav*>(used);
	}

	GameEngine::Behavior::MoveTo::create(name, element, node);

	tinyxml2::XMLElement* variable = element->FirstChildElement("position");
	if (variable != nullptr) {
		node->m_move2variable = true;
		node->m_destName = variable->GetText();
		node->m_variable_type = variable->Attribute("type");
	}
	return node;
}

//--------------------------------------

/**
 *
 */
void GameEngine::Behavior::SatNav::init(void *data)
{
	if (m_move2variable) {
		// load from a variable
		if (m_variable_type == "dequeu") {
			std::deque<glm::vec3>& playerLastPositions = m_tree->blackboard<std::deque<glm::vec3>>(m_destName);
			if (playerLastPositions.size() == 0) {
				BehaviorNode::m_status = BehaviorNode::Status::FAILED;
				return;
			}
			m_destination = playerLastPositions.back();
		}
		else if (m_variable_type == "vec3") {
			glm::vec3& playerLastPositions = m_tree->blackboard<glm::vec3>(m_destName);
			m_destination = playerLastPositions;
		}
	}
	else {
		glm::vec3* destination = static_cast<glm::vec3*>(data);
		m_destination = *destination;
	}

	m_navpoints.clear();
	float l = g_navMesh.findPath(m_entity->position(), m_destination, m_navpoints);

	if ( l > 0) {
		MoveTo::init(&m_navpoints);
	}
	else if (l == 0) {
		// we are already on final point
		BehaviorNode::m_status = BehaviorNode::Status::SUCCESSED;
	}
	else if (l <= 0) {
		// we can't move
		BehaviorNode::m_status = BehaviorNode::Status::ERR;
	}
}

/**
 *
 */
void GameEngine::Behavior::SatNav::onGoto(gaMessage* message)
{
	if (message->m_extra == nullptr) {
		m_destination = message->m_v3value;
	}
	else {
		m_destination = *(static_cast<glm::vec3*>(message->m_extra));
	}

	init(&m_destination);
}

/**
 * let a component deal with a situation
 */
void GameEngine::Behavior::SatNav::dispatchMessage(gaMessage* message, Action *r)
{
	switch (message->m_action) {
	case gaMessage::Action::SatNav_GOTO:
		onGoto(message);
		break;
	}

	MoveTo::dispatchMessage(message, r);
}

//----------------------------------

/**
 * display the component in the debugger
 */
void GameEngine::Behavior::SatNav::debugGUInode(void)
{
	if (m_move2variable) {
		ImGui::Text("%s(%s)", m_destName.c_str(), m_variable_type.c_str());
	}
	GameEngine::Behavior::MoveTo::debugGUInode();
}

//----------------------------------

/**
 * flight recorder
 */
uint32_t GameEngine::Behavior::SatNav::recordState(void* record)
{
	FlightRecorder::SatNav* r = static_cast<FlightRecorder::SatNav*>(record);

	MoveTo::recordState(record);

	uint32_t len = sizeof(FlightRecorder::SatNav);
	r->destination = m_destination;
	r->nbNavPoints = m_navpoints.size();

	uint32_t i = 0;
	for (uint32_t j = i; j < m_navpoints.size(); j++) {
		r->points[i++] = m_navpoints[j];
		len += sizeof(glm::vec3);
	}

	r->move2.node.size = len;
	return len;
}

uint32_t GameEngine::Behavior::SatNav::loadState(void* record)
{
	MoveTo::loadState(record);

	FlightRecorder::SatNav* r = static_cast<FlightRecorder::SatNav*>(record);

	m_destination = r->destination;
	m_navpoints.resize(r->nbNavPoints);

	uint32_t i = 0;
	for (uint32_t j = 0; j < r->nbNavPoints; j++) {
		m_navpoints[j] = r->points[i++];
	}
	return r->move2.node.size;
}
