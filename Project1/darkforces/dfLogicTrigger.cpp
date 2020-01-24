#include "dfLogicTrigger.h"

#include <iostream>
#include <string>

#include "dfLogicTrigger.h"
#include "dfSector.h"
#include "dfsign.h"
#include "dfMessageBus.h"

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
	dfMessageClient(sector),
	m_sector(sector)
{
	m_class = class2int(kind);
	m_name = sector;
	m_messages.push_back(dfMessage(DF_MESSAGE_TRIGGER, 0, m_name));
}

/**
 * Create a trigger without bounding box
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, std::string& sector, int wallIndex) :
	dfMessageClient(),
	m_sector(sector),
	m_wallIndex(wallIndex)
{
	m_class = class2int(kind);
	m_name = sector + "(" + std::to_string(wallIndex) + ")";
	addToBus();
}

/**
 * Create a trigger based on a wall of a sector, and record the elevator client
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfSector* sector, int wallIndex,  dfLogicElevator* client) :
	dfMessageClient(),
	m_wallIndex(wallIndex),
	m_sector(sector->m_name)
{
	m_clients.push_back(sector->m_name);
	sector->setTriggerFromWall(this);
	m_class = class2int(kind);
	m_name = sector->m_name + "(" + std::to_string(wallIndex) + ")";
	addToBus();
}

/**
 * Create a trigger based on the floor of a sector, and record the elevator client
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfSector* sector, dfLogicElevator* client) :
	dfMessageClient(sector->m_name),
	m_sector(sector->m_name)
{
	m_clients.push_back(sector->m_name);
	m_class = class2int(kind);

	// no sign => no trigger
}

/**
 * Create a trigger based on ono the sector managed by the elevator
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfLogicElevator* client):
	dfMessageClient()
{
	client->psector()->setTriggerFromSector(this);
	m_clients.push_back(client->sector());
	m_class = class2int(kind);
	m_sector = client->sector();
	m_name = client->sector() + "(0)";
	addToBus();

	// no sign => no trigger
}

/**
 * Add events to sectors
 */
void dfLogicTrigger::addEvents(dfSector* pSector)
{
	pSector->eventMask(m_eventMask);
}

void dfLogicTrigger::boundingBox(glm::vec2& left, glm::vec2& right, float floor, float ceiling)
{
	m_boundingBox = fwAABBox(
		left.x, right.x,
		left.y, right.y,
		floor, ceiling
	);

	m_boundingBoxCenter.x = (left.x + right.x) / 2.0f;
	m_boundingBoxCenter.y = (left.y + right.y) / 2.0f;
	m_boundingBoxCenter.z = (floor + ceiling) / 2.0f;

	m_boundingBoxSize.x = abs(left.x - right.x);
	m_boundingBoxSize.y = abs(left.y - right.y);
	m_boundingBoxSize.z = abs(ceiling - floor);
}

void dfLogicTrigger::boundingBox(fwAABBox& box)
{
	m_boundingBox = box;
}

/**
 * analyze the message to pass
 */
void dfLogicTrigger::message(std::vector<std::string>& tokens)
{
	dfMessage msg(tokens);
	m_messages.push_back(msg);
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
				m_messages.push_back(dfMessage(DF_MESSAGE_TRIGGER, 0, client));
			}
		}
	}
	else {
		// for every message, duplicate to every client
		dfMessage message;
		for (int i = m_messages.size() - 1; i >= 0; i--) {
			m_messages[i].m_client = m_clients[0];	// fix the first
			// add the next ones
			message = m_messages[i];
			for (unsigned int j = 1; j < m_clients.size(); j++) {
				message.m_client = m_clients[j];
				m_messages.push_back(message);
			}
		}
	}
}

/**
 * check the trigger colision box
 */
bool dfLogicTrigger::collide(fwAABBox& box)
{
	return m_boundingBox.intersect(box);
}

/**
 * move the boundingBox of the trigger
 */
void dfLogicTrigger::moveZ(float z)
{
	m_boundingBox.m_z = z;
	m_boundingBox.m_z1 = z + m_boundingBoxSize.z;
}

/**
 * Handle a message
 */
void dfLogicTrigger::dispatchMessage(dfMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_TRIGGER:
		activate();
		break;
	case DF_MESSAGE_DONE:
		if (m_pSign) {
			m_pSign->setStatus(0);	// turn the switch off
		}
		break;
	default:
		std::cerr << "dfLogicTrigger::dispatchMessage action " << message->m_action << " not implemented" << std::endl;
	}
}

/**
 * Handle the TRIGGER message
 */
void dfLogicTrigger::activate()
{
	if (m_pSign) {
		m_pSign->setStatus(1);	// turn the switch on
	}
	for (unsigned int i = 0; i < m_messages.size(); i++) {
		g_MessageBus.push(&m_messages[i]);
	}
}