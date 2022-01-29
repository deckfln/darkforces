#include "dfItem.h"

#include "../gaEngine/World.h"

#include "dfMessage.h"
#include "dfObject.h"
#include "dfObject/dfSpriteFME.h"
#include "dfObject/dfSprite/ienergy.h"
#include "dfObject/dfSprite/dfRifle.h"

#include "prefab/dfRedKey.h"

/**
 * Drop the item on the floor
 */
void DarkForces::Item::onDropItem(gaMessage* message)
{
	if (!m_droppable) {
		return;
	}

	DarkForces::Object* obj = nullptr;

	// constructor of a sprite expects a level space
	glm::vec3 p ;
	dfLevel::gl2level(message->m_v3value, p);

	switch (m_logic) {
	case dfLogic::DEAD_MOUSE:
		obj = new DarkForces::Sprite::FME("DEDMOUSE.FME", p, 1.0f);
		obj->hasCollider(false);
		obj->logic(dfLogic::DEAD_MOUSE);
		break;
	case dfLogic::ITEM_BATTERY:
		obj = new DarkForces::Sprite::FME("IBATTERY.FME", p, 1.0f);
		obj->hasCollider(true);
		obj->logic(dfLogic::ITEM_BATTERY);
		break;
	case dfLogic::ITEM_RIFLE:
		obj = new DarkForces::Sprite::Rifle(p, 1.0f, message->m_fvalue);
		obj->logic(dfLogic::ITEM_RIFLE);
		break;
	case dfLogic::ITEM_POWER:
		obj = new DarkForces::Sprite::FME("IPOWER.FME", p, 1.0f);
		obj->logic(dfLogic::ITEM_POWER);
		obj->hasCollider(true);
		break;
	case dfLogic::ITEM_ENERGY:
		obj = new IEnergy(p, 1.0f, message->m_fvalue);
		obj->logic(dfLogic::ITEM_ENERGY);
		break;
	case dfLogic::RED_KEY:
		obj = new DarkForces::Prefab::RedKey(p);
		break;
	}
	obj->physical(false);	// objects can be traversed and are not subject to gravity
	obj->gravity(false);

	g_gaWorld.addClient(obj);
}

/**
 *
 */
DarkForces::Item::Item(const std::string& name, uint32_t logic) :
	GameEngine::Item(name),
	m_logic(logic)
{

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