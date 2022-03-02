#include "dfCHUDText.h"

#include <imgui.h>
#include "../dfComponent.h"
#include "../dfMessage.h"
#include "../dfMsg.h"
#include "../dfFNT.h"

/**
 * display a text
 */
void DarkForces::Component::HUDtext::onText(gaMessage* message)
{
	Msg& msg = g_dfMsg.get(message->m_value);
	DarkForces::FNT::draw(m_hud, msg.text(), "SWFONT1.FNT");
}

DarkForces::Component::HUDtext::HUDtext(void) :
	gaComponent(DF_COMPONENT_HUDTEXT)
{
}

/**
 * create
 */
DarkForces::Component::HUDtext::HUDtext(fwTexture* hud):
	gaComponent(DF_COMPONENT_HUDTEXT),
	m_hud(hud)
{
}

/**
 * let the component deal with messages
 */
void DarkForces::Component::HUDtext::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::TEXT:
		onText(message);
		break;
	}
}

#ifdef _DEBUG
/**
 *  Add dedicated component debug the entity
 */
void DarkForces::Component::HUDtext::debugGUIinline(void)
{
	if (ImGui::TreeNode("dfCHUDText")) {
		ImGui::TreePop();
	}
}
#endif