#include "gaBTurn.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaComponent/gaBehaviorTree.h"

/**
 * test conditions
 */
 /**
  * check exit conditions
  */
bool GameEngine::Behavior::Turn::conditionMet(void)
{
	bool *condition;
	for (auto& exit : m_exit) {
		condition = m_tree->blackboard<bool>(exit.first);
		if (condition && *condition == exit.second) {
			return true;
		}
	}

	return false;
}

/**
 * "look around"
 */
void GameEngine::Behavior::Turn::onTimer(gaMessage* message)
{
	if (conditionMet()) {
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		g_gaWorld.registerTimerEvents(m_entity, false);
		return;
	}

	if (m_untilNextTurn-- == 0) {
		m_currentAngle++;
		if (m_currentAngle > m_angles.size()) {
			m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
			g_gaWorld.registerTimerEvents(m_entity, false);
			return;
		}

		float a = m_angles[m_currentAngle];
		m_entity->sendMessage(gaMessage::ROTATE, gaMessage::Flag::ROTATE_BY, a);
		m_untilNextTurn = m_delay * 33;
	}
}

//-------------------------------------

GameEngine::Behavior::Turn::Turn(const char* name) :
	BehaviorNode(name)
{
}

GameEngine::BehaviorNode* GameEngine::Behavior::Turn::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Turn* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Turn*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Turn(m_name);
	}

	cl->m_delay = m_delay;
	for (auto& condition : m_exit) {
		cl->m_exit[condition.first] = condition.second;
	}
	for (auto& angle : m_angles) {
		cl->m_angles.push_back(angle);
	}
	return cl;
}

void GameEngine::Behavior::Turn::init(void*)
{
	float a;
	m_currentAngle = 0;

	a = m_angles[m_currentAngle];

	m_entity->sendMessage(gaMessage::ROTATE, gaMessage::Flag::ROTATE_BY, a);

	m_untilNextTurn = m_delay * 33;

	g_gaWorld.registerTimerEvents(m_entity, true);
}

/**
 *
 */
GameEngine::BehaviorNode* GameEngine::Behavior::Turn::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Turn* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Turn(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Turn*>(used);
	}

	// get the delay
	tinyxml2::XMLElement* xmlDelay = element->FirstChildElement("delay");
	const char* cdelay = xmlDelay->GetText();
	if (cdelay) {
		node->m_delay = std::stoi(cdelay);
	}

	// load the exit conditions
	bool value = false;

	tinyxml2::XMLElement* exits = element->FirstChildElement("exits");
	if (exits) {
		tinyxml2::XMLElement* exit = exits->FirstChildElement("exit");

		while (exit != nullptr) {
			exit->QueryBoolAttribute("value", &value);
			node->m_exit[exit->GetText()] = value;
			exit = exit->NextSiblingElement("exit");
		}
	}

	// load the angles
	const double pi = 3.14159265359;
	tinyxml2::XMLElement* angles = element->FirstChildElement("angles");
	if (angles) {
		const char* cangle;
		tinyxml2::XMLElement* xmlAngle = angles->FirstChildElement("angle");

		while (xmlAngle != nullptr) {
			cangle = xmlAngle->GetText();
			if (cangle) {
				float a = std::stof(cangle) * (pi / 180.0);
				node->m_angles.push_back(a);
			}
			xmlAngle = xmlAngle->NextSiblingElement("angle");
		}
	}

	return node;
}

//----------------------------------------------

/**
 *
 */
void GameEngine::Behavior::Turn::dispatchMessage(gaMessage* message, Action* r)
{
	switch (message->m_action) {
	case gaMessage::Action::TIMER:
		onTimer(message);
		break;
	}
	GameEngine::BehaviorNode::dispatchMessage(message, r);
}

//---------------------------------------------

void GameEngine::Behavior::Turn::debugGUInode(void)
{
	if (m_exit.size() > 0) {
		if (ImGui::TreeNode("Conditions")) {
			for (auto& exit : m_exit) {
				ImGui::Text("%s:%d", exit.first.c_str(), exit.second);
			}
			ImGui::TreePop();
		}
	}

	ImGui::Text("Delay: %d", m_delay);
	ImGui::Text("Until: %d", m_untilNextTurn);
	ImGui::Text("Current: %d", m_currentAngle);

	if (ImGui::TreeNode("angles")) {
		for (auto angle : m_angles) {
			ImGui::Text("%f", angle);
		}
		ImGui::TreePop();
	}
}

//---------------------------------------------


uint32_t GameEngine::Behavior::Turn::recordState(void* record)
{
	FlightRecorder::Turn* r = static_cast<FlightRecorder::Turn*>(record);

	BehaviorNode::recordState(record);

	uint32_t len = sizeof(FlightRecorder::Turn);
	r->current = m_currentAngle;
	r->untilNextTurn= m_untilNextTurn;

	r->node.size = len;
	return len;
}

uint32_t GameEngine::Behavior::Turn::loadState(void* record)
{
	BehaviorNode::loadState(record);

	FlightRecorder::Turn* r = static_cast<FlightRecorder::Turn*>(record);

	m_currentAngle= r->current;
	m_untilNextTurn = r->untilNextTurn;

	return r->node.size;
}
