#include "dfLogicTrigger.h"

#include <iostream>

#include "dfLogicTrigger.h"
#include "dfSector.h"
#include "dfsign.h"

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
 * Create a trigger without bounding box
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, std::string& sector, int wallIndex) :
	m_sector(sector),
	m_wallIndex(wallIndex)
{
	m_class = class2int(kind);
}

/**
 * Create a trigger based on a wall of a sector, and record the elevator client
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfSector* sector, int wallIndex,  dfLogicElevator* client) :
	m_pSector(sector),
	m_wallIndex(wallIndex)
{
	m_pClients.push_back(client);
	sector->setTriggerFromWall(this);
	m_class = class2int(kind);
}

/**
 * Create a trigger based on the floor of a sector, and record the elevator client
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfSector* sector, dfLogicElevator* client) :
	m_pSector(sector)
{
	m_pClients.push_back(client);
	m_class = class2int(kind);
}

/**
 * Create a trigger based on ono the sector managed by the elevator
 */
dfLogicTrigger::dfLogicTrigger(std::string& kind, dfLogicElevator* client)
{
	client->psector()->setTriggerFromSector(this);
	m_pClients.push_back(client);
	m_class = class2int(kind);
}

/**
 * Bind to the sector object
 */
void dfLogicTrigger::bindSectorAndWall(dfSector* pSector)
{
	m_pSector = pSector;

	if (m_eventMask & DF_ELEVATOR_ENTER_SECTOR) {
		m_pSector->addTrigger(DF_ELEVATOR_ENTER_SECTOR, this);
	}
	if (m_eventMask & DF_ELEVATOR_LEAVE_SECTOR) {
		m_pSector->addTrigger(DF_ELEVATOR_LEAVE_SECTOR, this);
	}

	// record the wall and the sign (if there is a sign)
	if (m_wallIndex >= 0) {
		m_pWall = m_pSector->m_walls[m_wallIndex];
	}
}

/**
 * Bind the sign to the elevator it controls
 */
void dfLogicTrigger::bindSignToElevator(void)
{
	if (m_pWall) {
		m_pSign = m_pWall->sign();
		if (m_pSign) {
			m_pSign->setClass(m_class);
			for (auto pClient : m_pClients) {
				pClient->addSign(m_pSign);
			}
		}
	}
}

/**
 * record the evelator the trigger is bound to
 */
void dfLogicTrigger::evelator(dfLogicElevator* pClient)
{
	m_pClients.push_back(pClient);
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
	if (tokens[1] == gotostop) {
		dfMessage msg(DF_MESSAGE_GOTO_STOP, std::stoi(tokens[2]));
		m_messages.push_back(msg);
	}
	else {
		std::cerr << "dfLogicTrigger::message " << tokens[1] << " not implemented" << std::endl;
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

void dfLogicTrigger::activate(void)
{
	if (m_messages.size() > 1) {
		std::cerr << "dfLogicTrigger::activate 2+ messages not implemented" << std::endl;
		return;
	}

	if (m_messages.size() == 1) {
		for (auto pClient : m_pClients) {
			pClient->trigger(m_class, m_pSign, &m_messages[0]);
		}
	}
	else {
		for (auto pClient : m_pClients) {
			pClient->trigger(m_class, m_pSign, nullptr);
		}
	}
}
