#include "dfActor.h"

#include <GLFW/glfw3.h>
#include <imgui.h>

#include "dfSector.h"
#include "dfLevel.h"

static const char* g_className = "dfActor";

DarkForces::Actor::Actor(int mclass, const std::string& name, fwCylinder& cylinder, const glm::vec3& feet, float eyes, float ankle):
	gaActor(mclass, name, cylinder, feet, eyes, ankle)
{
	m_className = g_className;

	addComponent(&m_defaultAI);
	addComponent(&m_sound);
	addComponent(&m_weapon);
	m_weapon.set(DarkForces::Component::Weapon::Kind::Rifle);
}

DarkForces::Actor::Actor(flightRecorder::Entity* record):
	gaActor(record)
{
	m_className = g_className;
}

/**
 * bind the level
 */ 
void DarkForces::Actor::bind(dfLevel* level)
{
	m_level = level;
	m_defaultAI.bind(level);
}


/**
 * let an entity deal with a situation
 */
void DarkForces::Actor::dispatchMessage(gaMessage* message)
{
	switch (message->m_action)
	{
	case gaMessage::KEY:
		switch (message->m_value) {
		case GLFW_KEY_LEFT_CONTROL:
			sendInternalMessage(DarkForces::Message::FIRE, 0, &m_direction);
			break;

		case GLFW_KEY_F5:
			if (!m_headlight) {
				m_headlight = true;
			}
			else {
				m_headlight = false;
			}
			break;
		}
		break;

	}

	gaActor::dispatchMessage(message);
}

/**
 * return a record of the entity state (for debug)
 */
uint32_t DarkForces::Actor::recordState(void* record)
{
	flightRecorder::DarkForces::Actor* r = static_cast<flightRecorder::DarkForces::Actor*>(record);
	gaActor::recordState(&r->actor);
	r->actor.entity.size = sizeof(flightRecorder::DarkForces::Actor);
	r->actor.entity.classID = flightRecorder::TYPE::DF_ACTOR;

	if (m_defaultAI.currentSector() != nullptr) {
		strncpy_s(r->currentSector, m_defaultAI.currentSector()->name().c_str(), sizeof(r->currentSector));
	}
	else {
		r->currentSector[0] = 0;
	}

	return sizeof(flightRecorder::DarkForces::Actor);
}

/**
 * reload an actor state from a record
 */
void DarkForces::Actor::loadState(void* record)
{
	flightRecorder::DarkForces::Actor* r = (flightRecorder::DarkForces::Actor*)record;
	gaActor::loadState(&r->actor);
	if (r->currentSector[0] != 0) {
		m_defaultAI.currentSector(m_level->findSector(r->currentSector));
	}
	else {
		m_defaultAI.currentSector(nullptr);
	}
}

/**
 * Add dedicated component debug the entity
 */
void DarkForces::Actor::debugGUIChildClass(void)
{
	gaActor::debugGUIChildClass();

	if (ImGui::TreeNode("dfActor")) {
		ImGui::TreePop();
	}
}