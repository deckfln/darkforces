#include "dfBulletExplode.h"

dfBulletExplode::dfBulletExplode(const glm::vec3& position, float ambient) :
	dfSpriteAnimated("BULLEXP.WAX", position, ambient)
{
	m_loopAnimation = false;
	state(DF_STATE_ENEMY_MOVE);
}

/**
 * let an entity deal with a situation
 */
void dfBulletExplode::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_END_LOOP:
		// remove the explosition from the world
		g_gaWorld.sendMessageDelayed(m_name, "_world", GA_MSG_DELETE_ENTITY, 0, nullptr);
		break;
	}

	dfSpriteAnimated::dispatchMessage(message);
}

dfBulletExplode::~dfBulletExplode()
{
}
