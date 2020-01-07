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
	m_stops.push_back(stop);
}

/**
 * move the connected elevator to its HOLD position
 */
void dfLogicElevator::init(void)
{
	dfLogicStop* stop = m_stops[0];
	if (stop) {
		float altitude = 0;
		
		if (m_pSector) {
			m_pSector->originalFloor();	// for relative stop
		}

		if (m_class == "inv") {
			m_pSector->setAltitude(false, stop->z_position(altitude));
		}
		else if(m_class == "basic") {
			m_pSector->setAltitude(true, stop->z_position(altitude));
		}
		else {
			std::cerr << "dfLogicElevator unknown class " << m_class << std::endl;
		}
	}
}

dfLogicElevator::~dfLogicElevator(void)
{
	for (auto stop : m_stops) {
		delete stop;
	}
}
