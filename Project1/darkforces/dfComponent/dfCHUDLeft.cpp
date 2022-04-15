#include "dfCHUDLeft.h"

#include "../dfComponent.h"
#include "../dfBitmap.h"
#include "../dfFileSystem.h"
#include "../dfLevel.h"
#include "../dfFNT.h"

void DarkForces::Component::HUDLeft::lazyInit(void)
{
	// health display using default image2D
	m_statuslf = new dfBitmap(g_dfFiles, "STATUSLF.BM", static_cast<dfLevel*>(g_gaLevel)->palette());
	dfBitmapImage* image = m_statuslf->getImage(0);
	float w1 = image->m_width / 320.0f;
	float h1 = image->m_height / 200.0f;
	m_image = new GameEngine::Image2D(
		"darkforce:statuslt",
		glm::vec2(w1, h1),					// width
		glm::vec2(w1 - 1.0f, h1 - 1.0f),	// position
		m_statuslf->fwtexture()
	);
}

/**
 * adapt to nw screen size
 */
void DarkForces::Component::HUDLeft::onScreenSize(gaMessage* message)
{
	float ratio = message->m_fvalue / 1.6f;

	// relocate the health
	dfBitmapImage* image = m_statuslf->getImage(0);
	float w = image->m_width / 320.0f / ratio;
	float h = image->m_height / 200.0f;
	m_image->scale(glm::vec2(w, h));
	m_image->translate(glm::vec2(-1.0f + w, -1.0f + h));
}

static char tmp[32];

/**
 * display number of life
 */
void DarkForces::Component::HUDLeft::onLife(gaMessage* message)
{
	// clear the shieldbox and draw the new shield count
	m_statuslf->fwtexture()->box(32, 25, 15, 9, glm::ivec4(0, 0, 0, 255));
	snprintf(tmp, sizeof(tmp), "%03d", message->m_value);
	DarkForces::FNT::draw(m_statuslf->fwtexture(), tmp, "HelNum.fnt", 33, 26);
}

/**
 * display number of shield
 */
void DarkForces::Component::HUDLeft::onShield(gaMessage* message)
{
	// clear the shieldbox and draw the new shield count
	m_statuslf->fwtexture()->box(15, 25, 15, 9, glm::ivec4(0, 0, 0, 255));
	snprintf(tmp, sizeof(tmp), "%03d", message->m_value);
	DarkForces::FNT::draw(m_statuslf->fwtexture(), tmp, "ArmNum.fnt", 16, 26);
}

/**
 *
 */
DarkForces::Component::HUDLeft::HUDLeft(void) : 
	gaComponent(DF_COMPONENT_HUD_LEFT)
{

}

/**
 *
 */
void DarkForces::Component::HUDLeft::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::LIFE:
		onLife(message);
		break;
	case gaMessage::Action::SCREEN_RESIZE:
		onScreenSize(message);
		break;
	case DarkForces::Message::SHIELD:
		onShield(message);
		break;
	}
}

GameEngine::Image2D* DarkForces::Component::HUDLeft::getImage(void)
{
	if (m_statuslf == nullptr) {
		lazyInit();
	}
	return m_image;
}

DarkForces::Component::HUDLeft::~HUDLeft(void)
{
	delete m_statuslf;
	delete m_image;
}
