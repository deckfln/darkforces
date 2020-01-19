#include "dfLogicStop.h"

#include "dfSector.h"

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
	if (tokens.size() == 5) {
		// message from a stop
		if (tokens[3] == gotostop) {
			dfMessage msg(DF_MESSAGE_GOTO_STOP, std::stoi(tokens[4]), tokens[2]);
			m_messages.push_back(msg);
		}
		else {
			std::cerr << "dfLogicStop::message " << tokens[3] << " not implemented for stops" << std::endl;
		}
	}
	else if (tokens.size() == 3) {
		// message from a trigger
		if (tokens[1] == gotostop) {
			dfMessage msg(DF_MESSAGE_GOTO_STOP, std::stoi(tokens[2]));
			m_messages.push_back(msg);
		}
		else {
			std::cerr << "dfLogicStop::message " << tokens[1] << " not implemented for triggers" << std::endl;
		}
	}
	else {
		std::cerr << "dfLogicStop::message not implemented" << std::endl;
	}
}

/**
 *for every message of the stop, bind the elevator
 */
void dfLogicStop::bindMessage2Elevator(std::map<std::string, dfLogicElevator*>& hashElevators)
{
	for (auto &message : m_messages) {
		if (message.m_client != "") {
			message.m_pClient = hashElevators[message.m_client];
		}
	}
}

/**
 * Once the stop is being reached, send messages to recipients
 */
void dfLogicStop::sendMessages()
{
	for (unsigned i = 0; i < m_messages.size(); i++) {
		dfLogicElevator* elevator = m_messages[i].m_pClient;
		if (elevator) {
			elevator->trigger(DF_TRIGGER_STANDARD, &m_messages[i]);
		}
	}
}

float dfLogicStop::z_position(void)
{
	switch (m_flag) {
		case 9:
		case 17:
			return m_absolute;
		case 10:
		case 18:
			return m_pSector->m_floorAltitude + m_relatiave;	// relative to the floor of the source sector
		case 12:
		case 20:
			return m_pSector->m_floorAltitude;	// coy the floor of another sector
	}
	return 0;
}
