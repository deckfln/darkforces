#include "dfBulletExplode.h"

#include "../../gaEngine/World.h"

#include "../dfModel.h"
#include "../dfLevel.h"
#include "../dfVOC.h"
#include "../../config.h"

#include "../flightRecorder/frDarkForces.h"

static const char* g_className = "dfBulletExplode";

void dfBulletExplode::init(const glm::vec3& position, float ambient)
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

}

dfBulletExplode::dfBulletExplode(const glm::vec3& position, float ambient) :
	DarkForces::Object("BULLEXP.WAX", position)
{
	init(position, ambient);
}

dfBulletExplode::dfBulletExplode(flightRecorder::DarkForces::BulletExplode* r) :
	DarkForces::Object(&r->object)
{
	init(r->lvlposition, r->ambient);
}

/**
 * let an entity deal with a situation
 */
void dfBulletExplode::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::ANIM_END:
		// remove the explosion from the world
		g_gaWorld.sendMessageDelayed(m_name, "_world", gaMessage::DELETE_ENTITY, 0, nullptr);
		break;
	}

	DarkForces::Object::dispatchMessage(message);
}

//**************************************

/**
 * return a record of an actor state(for debug)
 */
uint32_t dfBulletExplode::recordState(void* r)
{
	flightRecorder::DarkForces::BulletExplode* record = static_cast<flightRecorder::DarkForces::BulletExplode*>(r);
	DarkForces::Object::recordState(&record->object);
	record->object.entity.classID = flightRecorder::DarkForces::ENTITY::BULLETEXP;
	record->object.entity.size = sizeof(flightRecorder::DarkForces::BulletExplode);
	record->lvlposition = m_position_lvl;
	record->ambient = m_ambient;

	return record->object.entity.size;
}

/**
 * reload an actor state from a record
 */
void dfBulletExplode::loadState(void* r)
{
	flightRecorder::DarkForces::BulletExplode* record = static_cast<flightRecorder::DarkForces::BulletExplode*>(r);
	DarkForces::Object::loadState(&record->object);
	m_position_lvl = record->lvlposition;
	m_ambient = record->ambient;
}

dfBulletExplode::~dfBulletExplode()
{
	delete m_sprite;
}
