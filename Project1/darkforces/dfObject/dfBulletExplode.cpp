#include "dfBulletExplode.h"

#include "../../gaEngine/World.h"

#include "../dfModel.h"
#include "../dfLevel.h"
#include "../dfVOC.h"
#include "../../config.h"

static const char* g_className = "dfBulletExplode";

dfBulletExplode::dfBulletExplode(const glm::vec3& position, float ambient) :
	DarkForces::Object("BULLEXP.WAX", position)
{
	m_className = g_className;

	m_logic.logic(dfLogic::ANIM);

	m_sprite = new DarkForces::Component::SpriteAnimated("BULLEXP.WAX", ambient);
	addComponent(m_sprite);
	sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_MOVE, 1.0f);	// deactivate the loop of the animation

	// origin of a bullet explosion is at the center of the sprite
	m_position_lvl = position;
	glm::vec3 p;
	dfLevel::level2gl(position, p);
	p.y -= m_source->sizeGL().y / 2;
	moveTo(m_position_lvl);

	// prepare the sound component
	m_sound.addSound(0, loadVOC("ex-tiny1.voc")->sound());
	m_sound.position(p);
	addComponent(&m_sound);
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

	case DarkForces::Message::ANIM_END:
		// remove the explosion from the world
		g_gaWorld.sendMessageDelayed(m_name, "_world", gaMessage::DELETE_ENTITY, 0, nullptr);
		break;
	}

	DarkForces::Object::dispatchMessage(message);
}

dfBulletExplode::~dfBulletExplode()
{
	delete m_sprite;
}
