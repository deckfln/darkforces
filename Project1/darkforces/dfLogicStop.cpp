#include "dfLogicStop.h"

#include <map>

#include "../gaEngine/World.h"

#include "dfSector.h"
#include "dfElevator.h"

static std::map<std::string, dfLogicStop::Action> _stops = {
	{ "hold", dfLogicStop::Action::HOLD},
	{ "terminate", dfLogicStop::Action::TERMINATE}
};

dfLogicStop::dfLogicStop(dfElevator* parent):
	m_parent(parent)
{
	m_name = "STOP:" + m_parent->name();
	m_pSector = parent->psector();
}

dfLogicStop::dfLogicStop(dfElevator* parent, float altitude, dfSector* sector, const std::string& action):
	m_parent(parent),
	m_flag(Flag::Relative | Flag::ActionAtStop),
	m_relatiave(altitude),
	m_pSector(sector)
{
	m_name = "STOP:" + m_parent->name();
	dfLogicStop::action(action);
}

dfLogicStop::dfLogicStop(dfElevator* parent, float altitude, dfSector* sector, float time):
	m_parent(parent),
	m_flag(Flag::Relative | Flag::TimeAtStop),
	m_relatiave(altitude),
	m_pSector(sector),
	m_animation_time(time)
{
	m_name = "STOP:" + m_parent->name();
}

dfLogicStop::dfLogicStop(dfElevator* parent, float altitude, const std::string& action):
	m_parent(parent),
	m_flag(Flag::Absolute | Flag::ActionAtStop),
	m_absolute(altitude)
{
	m_name = "STOP:" + m_parent->name();
	dfLogicStop::action(action);
	m_pSector = parent->psector();
}

dfLogicStop::dfLogicStop(dfSector* sector, float altitude, const std::string& action):
	m_flag(Flag::Absolute | Flag::ActionAtStop),
	m_absolute(altitude),
	m_pSector(sector),
	m_name ("STOP:" + sector->name())
{
	dfLogicStop::action(action);
}

dfLogicStop::dfLogicStop(dfSector* sector, float altitude, float time):
	m_flag(Flag::Absolute | Flag::TimeAtStop),
	m_absolute(altitude),
	m_animation_time(time),
	m_pSector(sector),
	m_name("STOP:" + sector->name())
{
}

dfLogicStop::dfLogicStop(const std::string& sector)
{
	m_name = "STOP:" + sector;
}

dfLogicStop::dfLogicStop(dfSector* sector):
	m_pSector(sector)
{
	m_name = "STOP:" + sector->name();
}

/**
 * connect the Stop to a sector (for relative altitude, or another sector altitude)
 */
void dfLogicStop::sector(dfSector* pSector)
{
	if (m_flag & Flag::Relative) {
		m_pSector = pSector;
	}
}

void dfLogicStop::action(const std::string& action)
{
	if (_stops.count(action) > 0) {
		m_flag |= Flag::ActionAtStop;
		m_action = _stops[action];
	}
}

/**
 * return a new position based on the source position and the refernce
 */
bool dfLogicStop::isTimeBased(void)
{
	int a = m_flag & Flag::TimeAtStop;  
	return a == 8;
}

/**
 * Record messages
 */
void dfLogicStop::message(gaMessage* message)
{
	if (message) {
		m_messages.push_back(message);
	}
}

/**
 * Once the stop is being reached, send messages to recipients
 */
void dfLogicStop::sendMessages()
{
	for (unsigned i = 0; i < m_messages.size(); i++) {
		g_gaWorld.sendMessage(m_name, m_messages[i]->m_client, m_messages[i]->m_action, m_messages[i]->m_value, nullptr);
	}
}

float dfLogicStop::z_position(dfElevator::Type elevatorClass)
{
	switch (m_flag) {
	case Flag::Absolute|Flag::TimeAtStop:
	case Flag::Absolute|Flag::ActionAtStop:
			return m_absolute;
	case Flag::Relative|Flag::TimeAtStop:
	case Flag::Relative|Flag::ActionAtStop:
		/*
		switch (elevatorClass) {
		case dfElevatorStatus::MOVE_FLOOR:
			return m_pSector->m_floorAltitude + m_relatiave;	// relative to the ceiling of the source sector
		case dfElevatorStatus::MOVE_CEILING:
			return m_pSector->m_floorAltitude + m_relatiave;	// relative to the ceiling of the source sector

		default:
			return m_pSector->m_floorAltitude + m_relatiave;	// relative to the ceiling of the source sector
		}
		*/
		return m_pSector->referenceFloor() + m_relatiave;	// relative to the ceiling of the source sector
	case Flag::BasedOnSector|Flag::TimeAtStop:
	case Flag::BasedOnSector|Flag::ActionAtStop:
		switch (elevatorClass) {
		case dfElevator::Type::MOVE_FLOOR:
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
		if (message->m_client != "") {
			sectors.push_back(message->m_client);
		}
	}
}

/**
 * clean up
 */
dfLogicStop::~dfLogicStop()
{
	for (auto message : m_messages) {
		delete message;
	}
}
