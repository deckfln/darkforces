#include "dfBulletExplode.h"

#include "../../gaEngine/World.h"

#include "../dfModel.h"
#include "../dfLevel.h"
#include "../dfVOC.h"
#include "../../config.h"

static const char* g_className = "dfBulletExplode";

dfBulletExplode::dfBulletExplode(const glm::vec3& position, float ambient) :
	dfSpriteAnimated("BULLEXP.WAX", position, ambient)
{
	m_className = g_className;

	// origin of a bullet explosion is at the center of the sprite
	m_position_lvl = position;
	glm::vec3 p;
	dfLevel::level2gl(position, p);
	p.y -= m_source->sizeGL().y / 2;
	moveTo(m_position_lvl);

	m_loopAnimation = false;
	m_logics |= dfLogic::ANIM;
	state(dfState::ENEMY_MOVE);

	// prepare the sound component
	m_sound.addSound(0, loadVOC("ex-tiny1.voc")->sound());
	m_sound.position(p);

	addComponent(&m_sound);
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
	case gaMessage::Action::WORLD_INSERT:
		sendInternalMessage(gaMessage::PLAY_SOUND, 0);
		break;

	case DarkForces::Message::END_LOOP:
		// remove the explosion from the world
		g_gaWorld.sendMessageDelayed(m_name, "_world", gaMessage::DELETE_ENTITY, 0, nullptr);
		break;
	}

	dfSpriteAnimated::dispatchMessage(message);
}

dfBulletExplode::~dfBulletExplode()
{
}
