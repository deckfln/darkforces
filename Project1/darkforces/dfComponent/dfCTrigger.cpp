#include "dfCTrigger.h"

#include <imgui.h>
#include "../../config.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaCInventory.h"
#include "../dfComponent.h"

static std::map<std::string, std::string> g_keys = {
	{ "red", "darkforce:redkey"},
	{ "blue", "darkforce:bluekey"},
	{ "yellow", "darkforce:yellowkey"}
};

/**
 *
 */
void DarkForces::Component::Trigger::onTrigger(gaMessage* message)
{
	// if a key is need to activate the trigger, ensure the owner has the key
	GameEngine::Component::Inventory* inventory = dynamic_cast<GameEngine::Component::Inventory*>(message->m_pServer->findComponent(gaComponent::Inventory));

	if (m_key != "") {
		if (inventory && inventory->isPresent(m_key)) {
			message->m_pServer->sendMessage(message->m_client, DarkForces::Message::TRIGGER, 0);
		}
	}
	else {
		message->m_pServer->sendMessage(message->m_client, DarkForces::Message::TRIGGER, 0);
	}
}

/**
 *
 */
DarkForces::Component::Trigger::Trigger(const std::string& key):
	gaComponent(DF_COMPONENT_TRIGGER)
{
	if (g_keys.count(key) > 0) {
		// convert he INF file key to a darkforces:item key
		m_key = g_keys[key];
	}
}

/**
 *
 */
void DarkForces::Component::Trigger::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::KEY:
		if (message->m_value == 32) {
			message->m_pServer->sendMessage(message->m_client, DarkForces::Message::TRIGGER, 0);
		}
		break;
	case gaMessage::Action::ACTIVATE:
		onTrigger(message);
		break;
	}
}

/**
 * display the component in the debugger
 */
void DarkForces::Component::Trigger::debugGUIinline(void)
{
	if (ImGui::TreeNode("Trigger")) {
		ImGui::TreePop();
	}
}