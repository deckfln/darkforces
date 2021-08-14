#include "dfBulletExplode.h"

#include "../../gaEngine/World.h"

#include "../dfModel.h"
#include "../dfLevel.h"

dfBulletExplode::dfBulletExplode(const glm::vec3& position, float ambient) :
	dfSpriteAnimated("BULLEXP.WAX", position, ambient)
{
	// origin of a bullet explosion is at the center of the sprite
	glm::vec3 p;
	dfLevel::level2gl(position, p);
	p.y -= m_source->sizeGL().y / 2;
	dfLevel::gl2level(p, m_position_lvl);
	moveTo(m_position_lvl);

	m_loopAnimation = false;
	state(dfState::ENEMY_MOVE);
}

/**
 * let an entity deal with a situation
 */
void dfBulletExplode::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_END_LOOP:
		// remove the explosition from the world
		g_gaWorld.sendMessageDelayed(m_name, "_world", gaMessage::DELETE_ENTITY, 0, nullptr);
		break;
	}

	dfSpriteAnimated::dispatchMessage(message);
}

dfBulletExplode::~dfBulletExplode()
{
}
