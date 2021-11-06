#include "dfComponentLogic.h"

#include "../../config.h"
#include "../dfObject.h"
#include <imgui.h>

dfComponentLogic::dfComponentLogic() :
	gaComponent(DF_COMPONENT_LOGIC),
	m_logics(0)
{
}

/**
 * add a logic
 */
void dfComponentLogic::logic(uint32_t logic)
{
	m_logics |= logic;
}

void dfComponentLogic::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::COLLIDE:
		if (m_logics & dfLogic::ITEM_SHIELD) {
			// ADD ARMOR
			if (message->m_pServer->findComponent(gaComponent::Actor)) {
				// if the collider is a DF_ACTOR
				// send shield from me to the actor
				m_entity->sendMessage(message->m_server, DarkForces::Message::ADD_SHIELD, DF_SHIELD_ENERGY, nullptr);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
		}
		else if (m_logics & dfLogic::ITEM_ENERGY) {
			// ADD ENERGY
			if (message->m_pServer->findComponent(gaComponent::Actor)) {
				// if the collider is a DF_ACTOR
				// send energy from me to the actor
				m_entity->sendMessage(message->m_server, DarkForces::Message::ADD_ENERGY, DF_ENERGY_ENERGY, nullptr);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
		}
		else if (m_logics & dfLogic::ITEM_RIFLE) {
			// pick a rifle and bullets
			if (message->m_pServer->findComponent(gaComponent::Actor)) {
				// if the collider is a DF_ACTOR
				// send shield from me to the actor
				m_entity->sendMessage(message->m_server, DarkForces::Message::PICK_RIFLE_AND_BULLETS, DF_SHIELD_ENERGY, nullptr);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
		}
		else if (m_logics & dfLogic::ITEM_BATTERY) {
			// pick a rifle and bullets
			if (message->m_pServer->findComponent(gaComponent::Actor)) {
				// if the collider is a DF_ACTOR
				// send shield from me to the actor
				m_entity->sendMessage(message->m_server, DarkForces::Message::ADD_BATTERY, DF_BATTERY_ENERGY, nullptr);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
		}
		
		break;
	case DarkForces::Message::HIT_BULLET:
		if (m_logics & dfLogic::SCENERY) {
			m_entity->sendMessage(DarkForces::Message::STATE, (int)dfState::SCENERY_ATTACK);
		}
		break;
	case DarkForces::Message::END_LOOP:
		// animation loop for an object reached it's end
		if (m_logics & DF_LOGIC_ENEMIES) {
			switch ((dfState)message->m_value) {
			case dfState::ENEMY_DIE_FROM_PUNCH:
			case dfState::ENEMY_DIE_FROM_SHOT:
				m_entity->sendMessage(DarkForces::Message::STATE, (int)dfState::ENEMY_LIE_DEAD);
			}
		}
		break;
	case DarkForces::Message::STATE:
		// trigger animation for enemy, unless the object is static or has no animation
		if (m_logics & DF_LOGIC_ENEMIES) {
			if ((dfState)message->m_value == dfState::ENEMY_LIE_DEAD) {
				m_entity->sendMessage(DarkForces::Message::DEAD);
			}
		}		
		break;
	case DarkForces::Message::DEAD:
		//drop the bag of the object when an enemy dies
		if (m_logics & DF_LOGIC_ENEMIES) {
			if (m_logics & (dfLogic::COMMANDO | dfLogic::TROOP)) {
				((DarkForces::Object*)m_entity)->drop(dfLogic::ITEM_RIFLE);
			}
			else if (m_logics & dfLogic::OFFICER) {
				if (m_logics & dfLogic::RED_KEY) {
					((DarkForces::Object*)m_entity)->drop(dfLogic::RED_KEY);
				}
				((DarkForces::Object*)m_entity)->drop(dfLogic::ITEM_ENERGY);
			}
			else if (m_logics & dfLogic::INTDROID) {
				((DarkForces::Object*)m_entity)->drop(dfLogic::ITEM_POWER);
			}
		}
		break;
	}
}

static std::map<uint32_t, const char*> debugLogic = {
	{1, "SCENERY"},
	{2, "ANIM"},
	{4, "OFFICER"},
	{8, "COMMANDO"},
	{16, "TROOP"},
	{32, "RED_KEY"},
	{64, "INTDROID"},
	{128, "ITEM_SHIELD"},
	{256, "ITEM_ENERGY"},
	{512, "LIFE"},
	{1024, "REVIVE"},
	{2048, "MOUSEBOT"},
	{4096, "KEY_TRIGGER"},
	{8192, "ITEM_RIFLE"},
	{16384, "ITEM_POWER"},
	{32768, "ITEM_BATTERY"},
	{65536, "DEAD_MOUSE"}
};

/**
 * display the component in the debugger
 */
void dfComponentLogic::debugGUIinline(void)
{
	if (ImGui::TreeNode("dfComponentLogic")) {
		std::string sLogic = "";
		for (auto& logic : debugLogic) {
			if (m_logics & logic.first) {
				sLogic.append("|");
				sLogic.append(logic.second);
			}
		}
		ImGui::Text("Logic: %s", sLogic.c_str());
		ImGui::TreePop();
	}
}

dfComponentLogic::~dfComponentLogic()
{
}
