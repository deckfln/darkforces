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
				((dfObject*)m_entity)->drop("IST-GUNI.FME");
			}
			else if (m_logics & DF_LOGIC_I_OFFICER) {
				if (m_logics & DF_LOGIC_RED_KEY) {
					((dfObject*)m_entity)->drop("IKEYR.FME");
				}
				((dfObject*)m_entity)->drop("IENERGY.FME");
			}
			else if (m_logics & DF_LOGIC_INTDROID) {
				((dfObject*)m_entity)->drop("IPOWER.FME");
			}
		}
		else if (m_logics & DF_LOGIC_MOUSEBOT) {
			((dfObject*)m_entity)->drop("DEDMOUSE.FME");
			((dfObject*)m_entity)->drop("IBATTERY.FME");
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
