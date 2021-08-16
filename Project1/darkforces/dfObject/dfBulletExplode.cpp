#include "dfBulletExplode.h"

#include "../../gaEngine/World.h"

#include "../dfModel.h"
#include "../dfLevel.h"

static const char* g_className = "dfBulletExplode";

dfBulletExplode::dfBulletExplode(const glm::vec3& position, float ambient) :
	dfSpriteAnimated("BULLEXP.WAX", position, ambient)
{
	m_class_name = g_className;

	// origin of a bullet explosion is at the center of the sprite
	glm::vec3 p;
	dfLevel::level2gl(position, p);
	p.y -= m_source->sizeGL().y / 2;
	dfLevel::gl2level(p, m_position_lvl);
	moveTo(m_position_lvl);

	m_loopAnimation = false;
	m_logics |= dfLogic::ANIM;
	state(dfState::ENEMY_MOVE);
}

dfBulletExplode::dfBulletExplode(flightRecorder::DarkForces::SpriteAnimated* record):
	dfSpriteAnimated((flightRecorder::DarkForces::SpriteAnimated*)record)
{
}

/**
 * let an entity deal with a situation
 */
void dfBulletExplode::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_END_LOOP:
		// remove the explosion from the world
		g_gaWorld.sendMessageDelayed(m_name, "_world", gaMessage::DELETE_ENTITY, 0, nullptr);
		break;
	}

	dfSpriteAnimated::dispatchMessage(message);
}

dfBulletExplode::~dfBulletExplode()
{
}
