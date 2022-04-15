#include "dfCHUDRight.h"

#include "../dfComponent.h"
#include "../dfBitmap.h"
#include "../dfFileSystem.h"
#include "../dfLevel.h"
#include "../dfFNT.h"

void DarkForces::Component::HUDRight::lazyInit(void)
{
	// ammo display using default image2D
	m_statuslf = new dfBitmap(g_dfFiles, "STATUSRT.BM", static_cast<dfLevel*>(g_gaLevel)->palette());
	dfBitmapImage* image = m_statuslf->getImage(0);
	float w = image->m_width / 320.0f / m_ratio;
	float h = image->m_height / 200.0f;
	m_image = new GameEngine::Image2D(
		"darkforce:statusrt",
		glm::vec2(w, h),					// width
		glm::vec2(1.0f - w, h - 1.0f),	// position
		m_statuslf->fwtexture()
	);
}

/**
 * adapt to nw screen size
 */
void DarkForces::Component::HUDRight::onScreenSize(gaMessage* message)
{
	m_ratio = message->m_fvalue / 1.6f;

	// relocate the health
	dfBitmapImage* image = m_statuslf->getImage(0);
	float w = image->m_width / 320.0f / m_ratio;
	float h = image->m_height / 200.0f;
	m_image->scale(glm::vec2(w, h));
	m_image->translate(glm::vec2(1.0f - w, -1.0f + h));
}

static char tmp[32];

/**
 * display number of ammo
 */
void DarkForces::Component::HUDRight::onAmmo(gaMessage* message)
{
	// clear the ammo box and draw the new ammo count
	m_statuslf->fwtexture()->box(11, 21, 37, 11, glm::ivec4(0, 0, 0, 255));
	snprintf(tmp, sizeof(tmp), "%03d", message->m_value);
	DarkForces::FNT::draw(m_statuslf->fwtexture(), tmp, "AMONUM.FNT", 13, 21);
}

/**
 *
 */
DarkForces::Component::HUDRight::HUDRight(void) :
	gaComponent(DF_COMPONENT_HUD_LEFT)
{

}

/**
 *
 */
void DarkForces::Component::HUDRight::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::SCREEN_RESIZE:
		onScreenSize(message);
		break;
	case DarkForces::Message::AMMO:
		onAmmo(message);
		break;
	}
}

GameEngine::Image2D* DarkForces::Component::HUDRight::getImage(void)
{
	if (m_statuslf == nullptr) {
		lazyInit();
	}
	return m_image;
}

DarkForces::Component::HUDRight::~HUDRight(void)
{
	delete m_statuslf;
	delete m_image;
}
