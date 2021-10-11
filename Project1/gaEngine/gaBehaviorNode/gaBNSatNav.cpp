#include "gaBNSatNav.h"

#include <map>
#include <imgui.h>

#include "../gaEntity.h"
#include "../gaNavMesh.h"
#include "../World.h"
#include "../gaComponent/gaBehaviorTree.h"

#include "../flightRecorder/frPathFinding.h"

GameEngine::Behavior::SatNav::SatNav(void) :
	MoveTo("SatNave")
{
}

/**
 * create the component
 */
GameEngine::Behavior::SatNav::SatNav(const char *name) :
	MoveTo(name)
{
}

GameEngine::Behavior::SatNav::SatNav(const char *name, float speed) :
	MoveTo(name, speed)
{
}

/**
 *
 */
void GameEngine::Behavior::SatNav::init(void *data)
{
	glm::vec3* destination = static_cast<glm::vec3*>(data);

	m_destination = *destination;

	m_navpoints.clear();

	if (g_navMesh.findPath(m_entity->position(), m_destination, m_navpoints) > 0) {
		MoveTo::init(&m_navpoints);
	}
	else {
		// inform everyone of the failure
		BehaviorNode::m_status = BehaviorNode::Status::FAILED;
		m_entity->sendMessage(gaMessage::SatNav_NOGO);
	}
}

/**
 *
 */
void GameEngine::Behavior::SatNav::onGoto(gaMessage* message)
{
	if (message->m_extra == nullptr) {
		m_destination = message->m_v3value;
	}
	else {
		m_destination = *(static_cast<glm::vec3*>(message->m_extra));
	}

	m_tree->blackboard("final_target", &m_destination);

	init(&m_destination);
}

/**
 * let a component deal with a situation
 */
void GameEngine::Behavior::SatNav::dispatchMessage(gaMessage* message, Action *r)
{
	switch (message->m_action) {
	case gaMessage::Action::SatNav_GOTO:
		onGoto(message);
		break;
	}

	MoveTo::dispatchMessage(message, r);
}

/**
 * flight recorder
 */
uint32_t GameEngine::Behavior::SatNav::recordState(void* record)
{
	FlightRecorder::SatNav* r = static_cast<FlightRecorder::SatNav*>(record);

	MoveTo::recordState(record);

	uint32_t len = sizeof(FlightRecorder::SatNav);
	r->destination = m_destination;
	r->nbNavPoints = m_navpoints.size();

	uint32_t i = 0;
	for (uint32_t j = i; j < m_navpoints.size(); j++) {
		r->points[i++] = m_navpoints[j];
		len += sizeof(glm::vec3);
	}

	r->move2.node.size = len;
	return len;
}

uint32_t GameEngine::Behavior::SatNav::loadState(void* record)
{
	MoveTo::loadState(record);

	FlightRecorder::SatNav* r = static_cast<FlightRecorder::SatNav*>(record);

	m_destination = r->destination;
	m_navpoints.resize(r->nbNavPoints);

	uint32_t i = 0;
	for (uint32_t j = 0; j < r->nbNavPoints; j++) {
		m_navpoints[j] = r->points[i++];
	}
	return r->move2.node.size;
}
