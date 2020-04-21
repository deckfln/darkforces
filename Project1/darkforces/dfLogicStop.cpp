#include "dfLogicStop.h"

#include <map>
#include "dfSector.h"
#include "dfMessageBus.h"
#include "dfLogicElevator.h"

static std::map<std::string, int> _stops = {
	{ "hold", DF_STOP_HOLD},
	{ "terminate", DF_STOP_TERMINATE}
};

dfLogicStop::dfLogicStop(dfLogicElevator* parent):
	m_parent(parent)
{
}

dfLogicStop::dfLogicStop(dfLogicElevator* parent, float altitude, dfSector* sector, std::string& action):
	m_parent(parent),
	m_flag(2 | 16),
	m_relatiave(altitude),
	m_pSector(sector)
{
	dfLogicStop::action(action);
}

dfLogicStop::dfLogicStop(dfLogicElevator* parent, float altitude, dfSector* sector, float time):
	m_parent(parent),
	m_flag(2 | 8),
	m_relatiave(altitude),
	m_pSector(sector),
	m_time(time)
{
}

dfLogicStop::dfLogicStop(dfLogicElevator* parent, float altitude, std::string& action):
	m_parent(parent),
	m_flag(1 | 16),
	m_absolute(altitude)
{
	dfLogicStop::action(action);
}

dfLogicStop::dfLogicStop(dfLogicElevator* parent, float altitude, float time):
	m_parent(parent),
	m_flag(1 | 8),
	m_absolute(altitude),
	m_time(time)
{
}

/**
 * coonect the Stop to a sector (for relative altitude, or another sector altitude)
 */
void dfLogicStop::sector(dfSector* pSector)
{
	if (m_flag & 2) {
		m_pSector = pSector;
	}
}

void dfLogicStop::action(std::string& action)
{
	if (_stops.count(action) > 0) {
		m_flag |= 16;
		m_action = _stops[action];
	}
}

/**
 * return a new position based on the source position and the refernce
 */
bool dfLogicStop::isTimeBased(void)
{
	int a = m_flag & 8;  
	return a == 8;
}

/**
 * Record messages
 */
void dfLogicStop::message(std::vector<std::string>& tokens)
{
	dfMessage msg(tokens);

	m_messages.push_back(msg);
}

void dfLogicStop::addMessage(dfMessage& message)
{
	m_messages.push_back(message);
}

/**
 * Once the stop is being reached, send messages to recipients
 */
void dfLogicStop::sendMessages()
{
	for (unsigned i = 0; i < m_messages.size(); i++) {
		m_messages[i].m_server = "STOP:" + m_parent->name();
		g_MessageBus.push(&m_messages[i]);
	}
}

float dfLogicStop::z_position(int elevatorClass)
{
	switch (m_flag) {
		case 9:
		case 17:
			return m_absolute;
		case 10:
		case 18:
			/*
			switch (elevatorClass) {
			case DF_ELEVATOR_MOVE_FLOOR:
				return m_pSector->m_floorAltitude + m_relatiave;	// relative to the ceiling of the source sector
			case DF_ELEVATOR_MOVE_CEILING:
				return m_pSector->m_floorAltitude + m_relatiave;	// relative to the ceiling of the source sector

			default:
				return m_pSector->m_floorAltitude + m_relatiave;	// relative to the ceiling of the source sector
			}
			*/
			return m_pSector->referenceFloor() + m_relatiave;	// relative to the ceiling of the source sector
		case 12:
		case 20:
			switch (elevatorClass) {
			case DF_ELEVATOR_MOVE_FLOOR:
				return m_pSector->referenceCeiling();	// coy the ceiling of another sector
			default:
				return m_pSector->referenceFloor();		// coy the floor of another sector
			}
	}
	return 0;
}

/**
 * Get a list of all sectors that can receive messages from the stop
 */
void dfLogicStop::getMessagesToSectors(std::list<std::string>& sectors)
{
	for (auto message : m_messages) {
		if (message.m_client != "") {
			sectors.push_back(message.m_client);
		}
	}
}
