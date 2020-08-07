#include "dfLogicTrigger.h"

#include <iostream>
#include <string>

#include "../config.h"

#include "../gaEngine/gaDebug.h"
#include "../gaEngine/gaWorld.h"
#include "../gaEngine/gaActor.h"

#include "dfComponent/dfComponentActor.h"
#include "dfLogicTrigger.h"
#include "dfSector.h"
#include "dfsign.h"
#include "dfMesh.h"
#include "dfLevel.h"

static const std::string switch1 = "switch1";
static const std::string standard = "standard";

static int class2int(std::string kind)
{
	if (kind == switch1) {
		return DF_TRIGGER_SWITCH1;
	}
	else if (kind == standard) {
		return DF_TRIGGER_STANDARD;
	}
	else {
		std::cerr << "dfLogicTrigger::dfLogicTrigger class " << kind << " not implemented" << std::endl;
		return -1;
	}
}

/**
 *Create a trigger without bounding box
 */
dfLogicTrigger::dfLogicTrigger(std::string & kind, std::string & sector) :
	gaEntity(DF_ENTITY_TRIGGER, sector),
	m_sector(sector)
{
	m_class = class2int(kind);
	m_messages.push_back(new gaMessage(DF_MESSAGE_TRIGGER, 0, m_name));
}

/**
 * Create a trigger without bounding box
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, std::string& sector, int wallIndex) :
	gaEntity(DF_ENTITY_TRIGGER, sector + "(" + std::to_string(wallIndex) + ")"),
	m_sector(sector),
	m_wallIndex(wallIndex)
{
	m_class = class2int(kind);
}

/**
 * Create a trigger based on a wall of a sector, and record the elevator client
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfSector* sector, int wallIndex,  dfLogicElevator* client) :
	gaEntity(DF_ENTITY_TRIGGER, sector->m_name + "(" + std::to_string(wallIndex) + ")"),
	m_wallIndex(wallIndex),
	m_sector(sector->m_name),
	m_keys(client->keys()),
	m_pElevator(client)
{
	m_clients.push_back(sector->m_name);
	sector->setTriggerFromWall(this);
	m_class = class2int(kind);
}

/**
 * Create a trigger based on the floor of a sector, and record the elevator client
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfSector* sector, dfLogicElevator* client) :
	gaEntity(DF_ENTITY_TRIGGER, sector->m_name),
	m_sector(sector->m_name),
	m_keys(client->keys()),
	m_pElevator(client)
{
	m_clients.push_back(sector->m_name);
	m_class = class2int(kind);
}

/**
 * Create a trigger based on ono the sector managed by the elevator
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfLogicElevator* client):
	gaEntity(DF_ENTITY_TRIGGER, client->sector() + "(0)"),
	m_keys(client->keys()),
	m_pElevator(client)
{
	client->psector()->setTriggerFromSector(this);
	m_clients.push_back(client->sector());
	m_class = class2int(kind);
	m_sector = client->sector();
}

/**
 * Add events to sectors
 */
void dfLogicTrigger::addEvents(dfSector* pSector)
{
	pSector->eventMask(m_eventMask);
}

/**
 * create a static bounding box (in gl space) from direct values (in Level space)
 */
void dfLogicTrigger::boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling)
{
	// extend flat panels to the 3d dimension (will help with collision detection)
	float lx, rx;
	if (left.x == right.x) {
		lx = left.x - 0.2f;
		rx = left.x + 0.2f;
	}
	else {
		lx = left.x;
		rx = right.x;
	}

	float ly, ry;
	if (left.y == right.y) {
		ly = left.y - 0.2f;
		ry = left.y + 0.3f;
	}
	else {
		ly = left.y;
		ry = right.y;
	}

	// set the lower corner as the position
	glm::vec3 p(lx, ly, floor);
	dfLevel::level2gl(p);

	// and extend the size
	glm::vec3 p1_gl(0, 0, 0);
	glm::vec3 p2_level(rx - lx, ry - ly, ceiling - floor);
	glm::vec3 p2_gl;
	dfLevel::level2gl(p2_level, p2_gl);

	m_modelAABB = fwAABBox(p1_gl, p2_gl);

	sendInternalMessage(GA_MSG_MOVE, 0, &p);
}

/**
 * create a static bounding box (in gl space) from another box (in Level space)
 */
void dfLogicTrigger::boundingBox(fwAABBox& box)
{
	// set the lower corner as the position
	glm::vec3 p(box.m_p);
	dfLevel::level2gl(p);

	// and extend the size
	glm::vec3 p_gl(0);
	glm::vec3 p1_gl;
	dfLevel::level2gl(box.m_p1, p1_gl);
	p1_gl = p1_gl - p;
	m_modelAABB = fwAABBox(p_gl, p1_gl);

	sendInternalMessage(GA_MSG_MOVE, 0, &p);
}

/**
 * bind the bounding box to an elevator (the elevator might move)
 */
void dfLogicTrigger::boundingBox(dfLogicElevator* elevator)
{
	m_pElevator = elevator;
}

/**
 * analyze the message to pass
 */
void dfLogicTrigger::message(gaMessage *msg)
{
	if (msg) {
		m_messages.push_back(msg);
	}
}

/**
 * Finalize the configuration of the trigger
 */
void dfLogicTrigger::config(void)
{
	if (m_messages.size() == 0) {
		if (m_clients.size() >= 0) {
			// inform all clients
			for (auto & client: m_clients) {
				m_messages.push_back(new gaMessage(DF_MESSAGE_TRIGGER, 0, client));
			}
		}
	}
	else {
		// for every message, duplicate to every client
		gaMessage* message;
		for (int i = m_messages.size() - 1; i >= 0; i--) {
			m_messages[i]->m_client = m_clients[0];	// fix the first
			// add the next ones
			message = m_messages[i];
			for (unsigned int j = 1; j < m_clients.size(); j++) {
				message->m_client = m_clients[j];
				m_messages.push_back(message);
			}
		}
	}
}

/**
 * move the boundingBox of the trigger when the floor of the sector moves
 */
void dfLogicTrigger::moveZ(float z)
{
	glm::vec3 p = position();
	p.y = z;
	moveTo(p);
}

/**
 * Move the bounding box of the trigger when the ceiling of the sector moves
 */
void dfLogicTrigger::moveCeiling(float z)
{
	glm::vec3 p = position();
	p.y = z - m_modelAABB.height();
	moveTo(p);
}

/**
 * Handle a message
 */
void dfLogicTrigger::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_TRIGGER:
		activate(message->m_server);
		break;
	case DF_MESSAGE_DONE:
		if (m_pMesh) {
			m_pMesh->setStatus(0);	// turn the switch off
		}
		m_actived = false;
		break;
	default:
#ifdef DEBUG
		gaDebugLog(REDUCED_DEBUG, "dfLogicTrigger::dispatchMessage",  "action " + std::to_string(message->m_action) + " not implemented");
#endif
	}

	gaEntity::dispatchMessage(message);
}

/**
 * bind the trigger to it's elevator
 */
void dfLogicTrigger::elevator(dfLogicElevator* elevator)
{
	if (m_pElevator == nullptr) {
		m_pElevator = elevator;
	}
#ifdef DEBUG
	else if (m_pElevator != elevator) {
		gaDebugLog(LOW_DEBUG, "dfLogicTrigger::elevator", "elevators are different");
	}
#endif
}

/**
 * Handle the TRIGGER message
 */
void dfLogicTrigger::activate(const std::string& activator)
{
	// the trigger was already activated
	if (m_actived) {
		return;
	}

#ifdef DEBUG
	gaDebugLog(REDUCED_DEBUG, "dfLogicTrigger::activate", m_name);
#endif

	if (m_pMesh) {
		m_pMesh->setStatus(1);	// turn the switch on
	}

	// verify the activator is an actor
	gaActor* entity = (gaActor*)g_gaWorld.getEntity(activator);
	if (entity==nullptr) {
		return;
	}
	dfComponentActor* actor = (dfComponentActor*)entity->findComponent(DF_COMPONENT_ACTOR);
	if (actor == nullptr) {
		return;
	}

	// check if there is no key needed or if the actor has the mandatory keys for the trigger
	if (m_keys == 0 || (m_keys & actor->keys()) != 0) {
		for (unsigned int i = 0; i < m_messages.size(); i++) {
			g_gaWorld.sendMessage(m_name, m_messages[i]->m_client, m_messages[i]->m_action, m_messages[i]->m_value, nullptr);
		}

		// only switches needs activation/deactivation
		if (m_class == DF_TRIGGER_SWITCH1) {
			m_actived = true;
		}
	}
}

/**
 * Clean up
 */
dfLogicTrigger::~dfLogicTrigger()
{
	for (auto message : m_messages) {
		delete message;
	}
}