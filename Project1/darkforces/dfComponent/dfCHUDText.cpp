#include "dfCHUDText.h"

#include <imgui.h>

#include "../../gaEngine/World.h"
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

	if (msg.importance() > m_importance) {
		uint32_t ticks;

		m_hud->clear();	// clear the background
		DarkForces::FNT::draw(m_hud, msg.text(), "glowing.fnt", 0, 1);

		// cancel existing alarm
		if (m_alarmID >= 0) {
			g_gaWorld.cancelAlarmEvent(m_alarmID);
		}

		m_importance = msg.importance();
		if (m_importance > 1) {
			ticks = 3000;
		}
		else {
			ticks = 8000;
		}
		GameEngine::Alarm alarm(m_entity, ticks);
		m_alarmID = g_gaWorld.registerAlarmEvent(alarm);
	}
}

/**
 * remove the text
 */
void DarkForces::Component::HUDtext::onAlarm(gaMessage* message)
{
	if (m_alarmID >= 0) {
		m_alarmID = -1;
		m_importance = -1;
		m_hud->clear();
	}
}

static char tmp[32];

/**
 * display number of ammo
 */
void DarkForces::Component::HUDtext::onAmmo(gaMessage* message)
{
	// clear the ammo box and draw the new ammo count
	m_ammo->box(11, 21, 37, 11, glm::ivec4(0, 0, 0, 255));
	snprintf(tmp, sizeof(tmp), "%03d", message->m_value);
	DarkForces::FNT::draw(m_ammo, tmp, "AMONUM.FNT", 13, 21);
}

/**
 * create
 */
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
	case gaMessage::ALARM:
		onAlarm(message);
		break;
	case DarkForces::Message::AMMO:
		onAmmo(message);
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