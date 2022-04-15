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
void DarkForces::Component::HUDtext::lazyInit(void)
{
	// text display using default image2D
	int32_t h, w, ch;
	uint8_t* data = m_text_bmp.get_info(&h, &w, &ch);
	if (data == nullptr) {
		m_text_bmp.data(new uint8_t[h * w * ch]);
		m_text_bmp.clear();
	}

	m_image = new GameEngine::Image2D(
		"text",
		glm::vec2(1.0f, 0.05f),
		glm::vec2(0.0f, 0.95f),
		&m_text_bmp
	);
}

void DarkForces::Component::HUDtext::onText(gaMessage* message)
{
	Msg& msg = g_dfMsg.get(message->m_value);

	if (msg.importance() > m_importance) {
		uint32_t ticks;

		m_text_bmp.clear();	// clear the background
		DarkForces::FNT::draw(&m_text_bmp, msg.text(), "glowing.fnt", 0, 1);

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
		m_text_bmp.clear();
	}
}

/**
 * adapt to nw screen size
 */
void DarkForces::Component::HUDtext::onScreenSize(gaMessage* message)
{
	float ratio = message->m_fvalue / 1.6f;
	int32_t h1, w1, ch1;

	// relocate the health
	m_text_bmp.get_info(&w1, &h1, &ch1);
	float w = w1 / 320.0f / ratio;
	float h = h1 / 200.0f;
	m_image->scale(glm::vec2(w, h));
	m_image->translate(glm::vec2(-1.0f + w, 1.0f - h));
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
	gaComponent(DF_COMPONENT_HUDTEXT)
{
}

DarkForces::Component::HUDtext::~HUDtext(void)
{
	delete m_image;
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
	case gaMessage::Action::SCREEN_RESIZE:
		onScreenSize(message);
		break;
	}
}

GameEngine::Image2D* DarkForces::Component::HUDtext::getImage(void)
{
	if (m_image == nullptr) {
		lazyInit();
	}
	return m_image;
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