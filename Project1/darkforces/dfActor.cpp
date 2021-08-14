#include "dfActor.h"

#include "dfSector.h"
#include "dfElevator.h"
#include "dfLevel.h"

#include <imgui.h>

DarkForces::Actor::Actor(int mclass, const std::string& name, fwCylinder& cylinder, const glm::vec3& feet, float eyes, float ankle):
	gaActor(mclass, name, cylinder, feet, eyes, ankle)
{
	addComponent(&m_defaultAI);
}

DarkForces::Actor::Actor(flightRecorder::Entity* record):
	gaActor(record)
{
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
void DarkForces::Actor::loadState(flightRecorder::Entity* record)
{
	flightRecorder::DarkForces::Actor* r = (flightRecorder::DarkForces::Actor*)record;
	gaActor::loadState((flightRecorder::Entity * )&r->actor);
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
	if (ImGui::TreeNode("dfActor")) {
		ImGui::TreePop();
	}
}