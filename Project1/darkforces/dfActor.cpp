#include "dfActor.h"

#include "dfSector.h"
#include "dfElevator.h"
#include "dfLevel.h"

#include <imgui.h>

DarkForces::Actor::Actor(int mclass, const std::string& name, fwCylinder& cylinder, const glm::vec3& feet, float eyes, float ankle):
	gaActor(mclass, name, cylinder, feet, eyes, ankle)
{
}

DarkForces::Actor::Actor(flightRecorder::Entity* record):
	gaActor(record)
{
}

/**
 * deal with messages for DF
 */
void DarkForces::Actor::dispatchMessage(gaMessage* message)
{
	switch (message->m_action)
	{
	case gaMessage::MOVE:
		// identity the DF sector and trigger enter/leave if changing
		if (message->m_extra == nullptr) {
			dfSector* current = m_level->findSector(position());
			if (current != m_currentSector) {
				if (m_currentSector != nullptr) {
					m_currentSector->event(dfElevator::Message::LEAVE_SECTOR);
				}
				if (current != nullptr) {
					current->event(dfElevator::Message::ENTER_SECTOR);
				}
				m_currentSector = current;
			}
		}
		break;
	}

	gaActor::dispatchMessage(message);
}

/**
 * return a record of the entity state (for debug)
 */
void DarkForces::Actor::recordState(void* record)
{
	flightRecorder::DarkForces::Actor* r = static_cast<flightRecorder::DarkForces::Actor*>(record);
	gaActor::recordState(&r->actor);
	r->actor.entity.size = sizeof(flightRecorder::DarkForces::Actor);
	r->actor.entity.classID = flightRecorder::TYPE::DF_ACTOR;

	if (m_currentSector != nullptr) {
		strncpy_s(r->currentSector, m_currentSector->m_name.c_str(), sizeof(r->currentSector));
	}
	else {
		r->currentSector[0] = 0;
	}
}

/**
 * reload an actor state from a record
 */
void DarkForces::Actor::loadState(flightRecorder::Entity* record)
{
	flightRecorder::DarkForces::Actor* r = (flightRecorder::DarkForces::Actor*)record;
	gaActor::loadState((flightRecorder::Entity * )&r->actor);
	if (r->currentSector[0] != 0) {
		m_currentSector = m_level->findSector(r->currentSector);
	}
	else {
		m_currentSector = nullptr;
	}
}

/**
 * Add dedicated component debug the entity
 */
void DarkForces::Actor::debugGUIChildClass(void)
{
	if (m_currentSector) {
		ImGui::Text("Sector:%s", m_currentSector->m_name.c_str());
	}
}