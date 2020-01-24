#include "dfLogicStop.h"

#include "dfSector.h"
#include "dfMessageBus.h"
#include "dfLogicElevator.h"

dfLogicStop::dfLogicStop()
{
}

dfLogicStop::dfLogicStop(float altitude, dfSector* sector, std::string& action)
{
	m_flag = 2 | 16;
	m_relatiave = altitude;
	m_pSector = sector;
	m_action = action;
}

dfLogicStop::dfLogicStop(float altitude, dfSector* sector, float time)
{
	m_flag = 2 | 8;
	m_relatiave = altitude;
	m_pSector = sector;
	m_time = time;
}

dfLogicStop::dfLogicStop(float altitude, std::string& action)
{
	m_flag = 1 | 16;
	m_absolute = altitude;
	m_action = action;
}

dfLogicStop::dfLogicStop(float altitude, float time)
{
	m_flag = 1 | 8;
	m_absolute = altitude;
	m_time = time;
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

/**
 * Once the stop is being reached, send messages to recipients
 */
void dfLogicStop::sendMessages()
{
	for (unsigned i = 0; i < m_messages.size(); i++) {
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
			switch (elevatorClass) {
			case DF_ELEVATOR_MOVE_FLOOR:
				return m_pSector->m_ceilingAltitude + m_relatiave;	// relative to the ceiling of the source sector
			default:
				return m_pSector->m_floorAltitude + m_relatiave;	// relative to the floor of the source sector
			}
		case 12:
		case 20:
			switch (elevatorClass) {
			case DF_ELEVATOR_MOVE_FLOOR:
				return m_pSector->m_ceilingAltitude;	// coy the ceiling of another sector
			default:
				return m_pSector->m_floorAltitude;		// coy the floor of another sector
			}
	}
	return 0;
}
