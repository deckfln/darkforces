#include "dfLogicElevator.h"

#include "dfSector.h"

dfLogicElevator::dfLogicElevator(std::string& kind, std::string& sector):
	m_class(kind),
	m_sector(sector)
{
}

/**
 * register the STOP in the correct list
 */
void dfLogicElevator::addStop(dfLogicStop* stop)
{
	if (stop->isTimeBased()) {
		m_timeStops.push_back(stop);
	}
	else {
		m_tokenSstops[stop->action()] = stop;
	}
}

/**
 * move the connected elevator to its HOLD position
 */
void dfLogicElevator::init(void)
{
	dfLogicStop* stop = m_tokenSstops["hold"];
	if (stop) {
		m_pSector->setCeilingAltitude(stop->z_position());
	}
}

dfLogicElevator::~dfLogicElevator(void)
{
	for (auto stop : m_timeStops) {
		delete stop;
	}
	for (auto stop : m_tokenSstops) {
		delete stop.second;
	}
}
