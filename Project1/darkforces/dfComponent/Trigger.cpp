#include "Trigger.h"

#include <imgui.h>
#include "../../config.h"
#include "../../gaEngine/gaEntity.h"

DarkForces::Component::Trigger::Trigger(void):
	gaComponent(DF_COMPONENT_TRIGGER)
{
}

/**
 *
 */
void DarkForces::Component::Trigger::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::KEY:
		if (message->m_value == 32) {
			message->m_pServer->sendMessage(message->m_client, DF_MESSAGE_TRIGGER, 0);
		}
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