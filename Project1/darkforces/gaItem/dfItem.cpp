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

	// does the item drop a DF entity ?
	if (!m_droppable) {
		return;
	}

	// constructor of a sprite expects a level space
	glm::vec3 p;
	glm::vec3 p1 = message->m_v3value + glm::vec3(2.0*message->m_fvalue / (rand() % 10) - message->m_fvalue, 0, 2.0 * message->m_fvalue / (rand() % 10) - message->m_fvalue);
	dfLevel::gl2level(p1, p);

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
	case dfLogic::ITEM_RIFLE:
		m_object = new DarkForces::Sprite::Rifle(p, 1.0f, (uint32_t)message->m_fvalue);
		m_object->logic(dfLogic::ITEM_RIFLE);
		break;
	case dfLogic::ITEM_POWER:
		m_object = new DarkForces::Sprite::FME("IPOWER.FME", p, 1.0f);
		m_object->logic(dfLogic::ITEM_POWER);
		m_object->hasCollider(true);
		break;
	case dfLogic::ITEM_ENERGY:
		m_object = new DarkForces::Prefab::Energy(p, (uint32_t)message->m_fvalue);
		break;
	case dfLogic::RED_KEY:
		m_object = new DarkForces::Prefab::RedKey(p);
		break;
	}
	m_object->item(this);
	m_object->physical(false);	// objects can be traversed and are not subject to gravity
	m_object->gravity(false);

	g_gaWorld.addClient(m_object);
}

/**
 *
 */
DarkForces::Item::Item(const std::string& name, uint32_t logic) :
	GameEngine::Item(name),
	m_logic(logic)
{
}

DarkForces::Item::Item(const char *name, uint32_t logic) :
	GameEngine::Item(name),
	m_logic(logic)
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