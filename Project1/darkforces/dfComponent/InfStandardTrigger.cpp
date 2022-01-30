#include "InfStandardTrigger.h"
#include <map>

#include "../../config.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaEntity.h"

#include "../dfComponent.h"

#include <imgui.h>

using namespace DarkForces::Component;

static std::map<const std::string, Event> _triggers = {
	{"standard", Event::DF_TRIGGER_STANDARD},
	{"switch1", Event::DF_TRIGGER_SWITCH1}
};

/**
 * execute the program
 */
void InfStandardTrigger::execute(void)
{
	for (unsigned int i = 0; i < m_messages.size(); i++) {
		g_gaWorld.sendMessage(m_entity->name(), m_messages[i]->m_client, 
			m_messages[i]->m_action, 
			m_messages[i]->m_value, 
			nullptr);
	}
}

InfStandardTrigger::InfStandardTrigger(const std::string& trigger, const std::string& sector) :
	gaComponent(DF_COMPONENT_INF)
{
	m_class = _triggers[trigger];
	m_sector = sector;
}
/**
 * compile all data into the final program
 */
void InfStandardTrigger::compile(void)
{
	if (m_messages.size() == 0) {
		// if there is no defined message
		for (auto& client : m_clients) {
			// send the default TRIGGER message to all clients
			m_messages.push_back(new gaMessage(DarkForces::Message::TRIGGER, 0, client));
		}
	}
	else {
		// for every message in the INF, duplicate to every client
		gaMessage* message;
		for (int i = m_messages.size() - 1; i >= 0; i--) {
			m_messages[i]->m_client = m_clients[0];	// fix the first
			// duplicate the next ones and change the client
			message = new gaMessage(m_messages[i]);
			for (unsigned int j = 1; j < m_clients.size(); j++) {
				message->m_client = m_clients[j];
				m_messages.push_back(message);
			}
		}
	}
}

/**
 *
 */
void InfStandardTrigger::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::EVENT:
		// m_action => event from DF
		uint32_t event = message->m_value;

		if ((m_eventMask & event) != 0) {
			// trigger the program if it fits the eventMask
			execute();
		}
		break;
	}
}

/**
 * display the component in the debugger
 */
void InfStandardTrigger::debugGUIinline(void)
{
	if (ImGui::TreeNode("InfProgram")) {
		std::string events;
		if (m_eventMask & DarkForces::MessageEvent::ENTER_SECTOR) {
			events = "ENTER_SECTOR";
		}
		if (m_eventMask & DarkForces::MessageEvent::LEAVE_SECTOR) {
			events += "|LEAVE_SECTOR";
		}

		static std::map<uint32_t, const char*> actions = {
			{DarkForces::Message::TRIGGER, "dfTrigger"},
			{DarkForces::Message::GOTO_STOP, "dfGotoStop"},
			{DarkForces::Message::DONE, "dfDone"},
			{DarkForces::Message::ADD_SHIELD, "dfAddShild"},
			{DarkForces::Message::ADD_ENERGY, "dfAddEnergy"},
			{DarkForces::Message::ANIM_LASTFRAME, "dfEndLoop"},		// the animation loop stopped
			{DarkForces::Message::DEAD, "dfDies"},			// the object died
			{DarkForces::Message::STATE, "dfState"},				// change the state of the object
			{DarkForces::Message::ADD_BATTERY, "dfAddBattery"},
			{DarkForces::Message::EVENT, "dfEvent"}				// send events to sectors
		};

		ImGui::Text("Events %s", events.c_str());
		for (auto message : m_messages) {
			ImGui::Text("To: %s", message->m_client.c_str());
			ImGui::SameLine();
			ImGui::Text("Action:%s", actions[message->m_action]);
			ImGui::SameLine();
			ImGui::Text("Value:%d", message->m_value);
		}
		ImGui::TreePop();
	}
}

InfStandardTrigger::~InfStandardTrigger()
{
	for (unsigned int i = 0; i < m_messages.size(); i++) {
		delete m_messages[i];
	}
}
