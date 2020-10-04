#include "dfComponentLogic.h"

#include "../../config.h"
#include "../dfObject.h"

dfComponentLogic::dfComponentLogic() :
	gaComponent(DF_COMPONENT_LOGIC),
	m_logics(0)
{
}

/**
 * add a logic
 */
void dfComponentLogic::logic(int logic)
{
	m_logics |= logic;
}

void dfComponentLogic::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::COLLIDE:
		if (m_logics & DF_LOGIC_ITEM_SHIELD) {
			// ADD ARMOR
			if (message->m_pServer->findComponent(DF_COMPONENT_ACTOR)) {
				// if the collider is a DF_ACTOR
				// send shield from me to the actor
				m_entity->sendMessage(message->m_server, DF_MESSAGE_ADD_SHIELD, DF_SHIELD_ENERGY, nullptr);

				// and remove the object from the scene
				m_entity->moveTo(glm::vec3(0));
			}
		}
		else if (m_logics & DF_LOGIC_ITEM_ENERGY) {
			// ADD ENERGY
			if (message->m_pServer->findComponent(DF_COMPONENT_ACTOR)) {
				// if the collider is a DF_ACTOR
				// send energy from me to the actor
				m_entity->sendMessage(message->m_server, DF_MESSAGE_ADD_ENERGY, DF_ENERGY_ENERGY, nullptr);

				// and remove the object from the scene
				m_entity->moveTo(glm::vec3(0));
			}
		}
		else if (m_logics & DF_LOGIC_ITEM_RIFLE) {
			// pick a rifle and bullets
			if (message->m_pServer->findComponent(DF_COMPONENT_ACTOR)) {
				// if the collider is a DF_ACTOR
				// send shield from me to the actor
				m_entity->sendMessage(message->m_server, DF_MSG_PICK_RIFLE_AND_BULLETS, DF_SHIELD_ENERGY, nullptr);

				// and remove the object from the scene
				m_entity->moveTo(glm::vec3(0));
			}
		}
		break;
	case DF_MESSAGE_HIT_BULLET:
		if (m_logics & DF_LOGIC_SCENERY) {
			m_entity->sendInternalMessage(DF_MSG_STATE, DF_STATE_SCENERY_ATTACK);
		}
		else if (m_logics & DF_LOGIC_MOUSEBOT) {
			m_entity->sendInternalMessage(DF_MESSAGE_DIES);
		}
		break;
	case DF_MESSAGE_END_LOOP:
		// animation loop for an object reached it's end
		if (m_logics & DF_LOGIC_ENEMIES) {
			switch (message->m_value) {
			case DF_STATE_ENEMY_DIE_FROM_PUNCH:
			case DF_STATE_ENEMY_DIE_FROM_SHOT:
				m_entity->sendInternalMessage(DF_MSG_STATE, DF_STATE_ENEMY_LIE_DEAD);
			}
		}
		break;
	case DF_MSG_STATE:
		// trigger animation for enemy, unless the object is static or has no animation
		if (m_logics & DF_LOGIC_ENEMIES) {
			if (message->m_value == DF_STATE_ENEMY_LIE_DEAD) {
				m_entity->sendInternalMessage(DF_MESSAGE_DIES);
			}
		}		
		break;
	case DF_MESSAGE_DIES:
		//drop the bag of the object when an enemy dies
		if (m_logics & DF_LOGIC_ENEMIES) {
			if (m_logics & (DF_LOGIC_COMMANDO | DF_LOGIC_TROOP)) {
				((dfObject*)m_entity)->drop(DF_LOGIC_ITEM_RIFLE);
			}
			else if (m_logics & DF_LOGIC_I_OFFICER) {
				if (m_logics & DF_LOGIC_RED_KEY) {
					((dfObject*)m_entity)->drop(DF_LOGIC_RED_KEY);
				}
				((dfObject*)m_entity)->drop(DF_LOGIC_ITEM_ENERGY);
			}
			else if (m_logics & DF_LOGIC_INTDROID) {
				((dfObject*)m_entity)->drop(DF_LOGIC_ITEM_POWER);
			}
		}
		else if (m_logics & DF_LOGIC_MOUSEBOT) {
			((dfObject*)m_entity)->drop(DF_LOGIC_DEAD_MOUSE);
			((dfObject*)m_entity)->drop(DF_LOGIC_ITEM_BATTERY);
			// 3D objects being registered in dfParserObject cannot be deleted, so move away
			m_entity->moveTo(glm::vec3(0));
			break;
		}
		break;
	}
}

dfComponentLogic::~dfComponentLogic()
{
}
