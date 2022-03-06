#include "dfItem.h"

#include "../../gaEngine/World.h"

#include "../dfMessage.h"
#include "../dfObject.h"
#include "../dfObject/dfSpriteFME.h"
#include "../dfObject/dfSprite/dfRifle.h"

#include "../prefab/dfRedKey.h"
#include "../prefab/dfEnergy.h"

/**
 * Drop the item on the floor
 */
void DarkForces::Item::onDropItem(gaMessage* message)
{
	// unlink from parent inventory
	inventory(nullptr);

	if (!m_droppable) {
		return;
	}

	//drop the item somewhere on the outside of the entity
	glm::vec3 p;
	float deltaX = (rand() % 10) + 1;	// avoid the 0 value
	float deltaY = (rand() % 10) + 1;

	glm::vec3 delta(2.0 * message->m_fvalue / deltaX - message->m_fvalue, 0, 2.0 * message->m_fvalue / deltaY - message->m_fvalue);
	glm::vec3 p1 = message->m_v3value + delta;

	// constructor of a sprite expects a level space
	dfLevel::gl2level(p1, p);
	drop(p);
}

/**
 *
 */
DarkForces::Item::Item(const std::string& name, uint32_t logic, bool droppable) :
	GameEngine::Item(name),
	m_logic(logic),
	m_droppable(droppable)
{
}

DarkForces::Item::Item(const char *name, uint32_t logic, bool droppable) :
	GameEngine::Item(name),
	m_logic(logic),
	m_droppable(droppable)
{

}

void DarkForces::Item::Set(const char* name, uint32_t logic)
{
	GameEngine::Item::set(name);
	m_logic = logic;
}

DarkForces::Item::~Item(void)
{
	if (m_object) {
		delete m_object;
	}
}

/**
 *
 */
void DarkForces::Item::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::DROP_ITEM:
		onDropItem(message);
		break;
	}
}

/**
 *
 */
void DarkForces::Item::drop(const glm::vec3& p)
{
	switch (m_logic) {
	case dfLogic::DEAD_MOUSE:
		m_object = new DarkForces::Sprite::FME("DEDMOUSE.FME", p, 1.0f);
		m_object->hasCollider(false);
		m_object->logic(dfLogic::DEAD_MOUSE);
		break;
	case dfLogic::ITEM_BATTERY:
		m_object = new DarkForces::Sprite::FME("IBATTERY.FME", p, 1.0f);
		m_object->hasCollider(true);
		m_object->logic(dfLogic::ITEM_BATTERY);
		break;
	case dfLogic::ITEM_POWER:
		m_object = new DarkForces::Sprite::FME("IPOWER.FME", p, 1.0f);
		m_object->logic(dfLogic::ITEM_POWER);
		m_object->hasCollider(true);
		break;
	case dfLogic::ITEM_ENERGY:
		m_object = new DarkForces::Prefab::Energy(p, 0);
		break;
	case dfLogic::RED_KEY:
		m_object = new DarkForces::Prefab::RedKey(p);
		break;
	default:
		__debugbreak();
	}
	m_object->item(this);
	m_object->physical(false);	// objects can be traversed and are not subject to gravity
	m_object->gravity(false);

	GameEngine::World::add(m_object);
}
