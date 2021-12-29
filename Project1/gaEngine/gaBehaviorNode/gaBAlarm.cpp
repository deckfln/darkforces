#include "gaBAlarm.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaBehavior.h"
#include "../gaComponent/gaBehaviorTree.h"

//-------------------------------------

GameEngine::Behavior::Alarm::Alarm(const char* name) :
	BehaviorNode(name)
{
}

BehaviorNode* GameEngine::Behavior::Alarm::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Alarm* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Alarm*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Alarm(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	cl->m_minDelay = m_minDelay;
	cl->m_maxDelay = m_maxDelay;
	cl->m_message = m_message;

	return cl;
}

/**
 *
 */
GameEngine::BehaviorNode* GameEngine::Behavior::Alarm::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Alarm* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Alarm(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Alarm*>(used);
	}
	GameEngine::BehaviorNode::create(name, element, node);

	// get the variable name
	tinyxml2::XMLElement* xmlVariable = element->FirstChildElement("message");
	if (xmlVariable) {
		const char* cname = xmlVariable->GetText();
		int32_t msgID = GameEngine::Behavior::getMessage(cname);
		if (msgID < 0) {
			gaDebugLog(1, "GameEngine::Behavior::Alarm", "unknown message " + std::string(cname));
			exit(-1);
		}
		node->m_message = msgID;
	}

	tinyxml2::XMLElement* xmlTimer = element->FirstChildElement("timer");
	if (xmlTimer) {
		xmlTimer->QueryIntAttribute("min", &node->m_minDelay);
		xmlTimer->QueryIntAttribute("max", &node->m_maxDelay);
	}

	if (node->m_maxDelay == 0)  {
		gaDebugLog(1, "GameEngine::Behavior::Alarm", "max = 0");
		exit(-1);
	}

	return node;
}

//----------------------------------------------

void GameEngine::Behavior::Alarm::init(void*)
{
	m_timer = (rand() % (m_maxDelay - m_minDelay)) + m_minDelay;
	GameEngine::Alarm alarm(m_entity, m_timer, m_message);
	m_alarmID = g_gaWorld.registerAlarmEvent(alarm);

	m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
}

//----------------------------------------------

void GameEngine::Behavior::Alarm::debugGUInode(void)
{
	ImGui::Text("max:%d", m_maxDelay);
	ImGui::Text("min:%d", m_minDelay);
	ImGui::Text("rnd:%d", m_timer);
}