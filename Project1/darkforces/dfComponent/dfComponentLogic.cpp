#include "dfComponentLogic.h"

#include "../../config.h"
#include "../dfObject.h"
#include "../gaItem/dfItem/dfWeapon.h"
#include "../dfComponent.h"
#include "dfCActor.h"
#include "../gaItem/dfItem/dfHeadlight.h"

#include <imgui.h>

dfComponentLogic::dfComponentLogic() :
	gaComponent(DF_COMPONENT_LOGIC),
	m_logics(0)
{
}

namespace DarkForces {
	struct gItem {
		uint32_t kind;
		uint32_t value;
	};
}

static std::map<uint32_t, DarkForces::gItem> g_items = {
	{dfLogic::ITEM_ENERGY, {dfLogic::ITEM_ENERGY, 15}},
	{dfLogic::ITEM_SHIELD, {dfLogic::ITEM_SHIELD, 100}}
};

/**
 * add a logic
 */
void dfComponentLogic::logic(uint32_t logic)
{
	m_logics |= logic;

	if (g_items.count(logic) > 0) {
		m_value = g_items[logic].value;
	}
}

void dfComponentLogic::dispatchMessage(gaMessage* message)
{
	DarkForces::Component::Actor* actor;

	switch (message->m_action) {
	case gaMessage::Action::COLLIDE:
		actor = dynamic_cast<DarkForces::Component::Actor *>(message->m_pServer->findComponent(gaComponent::Actor));

		// only grant ressources for alive actors
		if (actor && !actor->dying()) {
			if (m_logics & dfLogic::ITEM_SHIELD) {
				// if the collider is a DF_ACTOR
				// send shield from me to the actor
				m_entity->sendMessage(message->m_server, DarkForces::Message::ADD_SHIELD, m_value, nullptr);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
			else if (m_logics & dfLogic::ITEM_ENERGY) {
				// ADD ENERGY
				// if the collider is a DF_ACTOR
				// send energy from me to the actor
				m_entity->sendMessage(message->m_server, DarkForces::Message::ADD_ENERGY, m_value, nullptr);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
			else if (m_logics & dfLogic::ITEM_RIFLE) {
				DarkForces::Object* object = dynamic_cast<DarkForces::Object*>(m_entity);
				GameEngine::Item* item = object->item();

				// pick a rifle and bullets
				// if the collider is a DF_ACTOR
				// send shield from me to the actor
				m_entity->sendMessage(message->m_server, gaMessage::Action::ADD_ITEM, 0, item);
				m_entity->sendMessage(DarkForces::Message::ADD_ENERGY, m_value);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
			else if (m_logics & dfLogic::ITEM_BATTERY) {
				// pick a rifle and bullets
				// if the collider is a DF_ACTOR
				// send shield from me to the actor
				m_entity->sendMessage(message->m_server, DarkForces::Message::ADD_BATTERY, DF_BATTERY_ENERGY, nullptr);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
			else if (m_logics & dfLogic::GOGGLES) {
				DarkForces::Headlight* goggles = new DarkForces::Headlight("goggles");
				m_items.push_back(goggles);

				// pick the googles and add to the inventory
				m_entity->sendMessage(message->m_server, gaMessage::ADD_ITEM, 0, goggles);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
			else if (m_logics & dfLogic::RED_KEY) {
				// transfert the redkey to collider
				DarkForces::Object* object = dynamic_cast<DarkForces::Object*>(m_entity);
				GameEngine::Item* redkey = object->item();
				m_entity->sendMessage(message->m_server, gaMessage::ADD_ITEM, 0, redkey);

				// and remove the object from the scene
				m_entity->sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			}
		}
		break;

	case gaMessage::Action::BULLET_HIT:
		if (m_logics & dfLogic::SCENERY) {
			m_entity->sendMessage(DarkForces::Message::STATE, (int)dfState::SCENERY_ATTACK);
		}
		break;

	case DarkForces::Message::ANIM_LASTFRAME:
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

	case DarkForces::Message::DYING:
		//drop the bag of the object when an enemy dies
		if (m_logics & DF_LOGIC_ENEMIES) {
			if (m_logics & dfLogic::INTDROID) {
				m_entity->sendMessage(gaMessage::Action::DROP_ITEM, dfLogic::ITEM_POWER);
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
		ImGui::Text("Value: %d", m_value);
		ImGui::TreePop();
	}
}

dfComponentLogic::~dfComponentLogic()
{
	for (auto item : m_items) {
		delete item;
	}
}
