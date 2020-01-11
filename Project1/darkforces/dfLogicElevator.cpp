#include "dfLogicElevator.h"

#include "dfSector.h"
#include "dfLevel.h"

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
void dfLogicElevator::init(int stopID)
{
	dfLogicStop* stop = m_stops[stopID];
	float floor = getFloorPosition(stop);

	if (m_pSector) {
		m_pSector->m_floorAltitude = floor;
		m_pSector->m_ceilingAltitude = floor + m_pSector->height();
	}
	else {
		std::cerr << "dfLogicElevator::init " << m_sector << " not linked" << std::endl;
	}
}

/**
 * Handle a trigger
 */
void dfLogicElevator::trigger(std::string& sclass)
{
	if (sclass != "switch1") {
		std::cerr << "dfLogicElevator::trigger unknown class " << sclass << std::endl;
		return;
	}

	if (m_status != DF_ELEVATOR_HOLD) {
		// let the elevator finish it's animation
		return;
	}

	// for speed = 0, move instantly to the next stop
	if (m_speed == 0) {
		std::cerr << "dfLogicElevator::trigger speed==0 not implemented" << std::endl;
	}

	m_parent->activateElevator(this);
	animate(0);
}

/**
 * Move the evelator to it's next stop at the defined speed
 */
bool dfLogicElevator::animate(time_t delta)
{
	m_tick += delta;

	switch (m_status) {
	case DF_ELEVATOR_HOLD:
		m_status = DF_ELEVATOR_MOVE;
		move2nextFloor();
		break;

	case DF_ELEVATOR_MOVE: {
		float current = m_pSector->moveFloor(m_direction);
		if (abs(current - m_target) < 0.1) {
			m_tick = 0;
			m_currentStop = m_nextStop;

			// force the altitude to get ride of math round
			float floor = getFloorPosition(m_stops[m_currentStop]);
			 m_pSector->setFloor(floor);

			if (m_stops[m_currentStop]->isTimeBased()) {
				// put the elevator on wait
				m_status = DF_ELEVATOR_WAIT;
			}
			else {
				std::string& action = m_stops[m_currentStop]->action();
				if (action == "hold") {
					m_status = DF_ELEVATOR_HOLD;
					return true;	// stop the animation
				}
				else {
					std::cerr << "dfLogicElevator::animate action " << action << " not implemented" << std::endl;
				}
			}
		}
		break;
		}

	case DF_ELEVATOR_WAIT:
		if (m_tick >= m_stops[m_currentStop]->time()) {
			move2nextFloor();
			m_status = DF_ELEVATOR_MOVE;
		}
		break;

	default:
		std::cerr << "dfLogicElevator::animate unknown status " << m_status << std::endl;
	}

	return false;
}

/**
 * Compute a floor altitude based on elevator kind and stop
 */
float dfLogicElevator::getFloorPosition(dfLogicStop* stop)
{
	float altitude = 0;

	if (m_pSector) {
		altitude = m_pSector->originalFloor();	// for relative stop based on original floor
	}

	float floor = 0;

	if (m_class == "inv") {
		floor = stop->z_position(altitude) - m_pSector->height();
	}
	else if (m_class == "basic") {
		floor = stop->z_position(altitude);
	}
	else {
		std::cerr << "dfLogicElevator unknown class " << m_class << std::endl;
	}

	return floor;
}

/**
 * compute the move to the next floor
 */
void dfLogicElevator::move2nextFloor(void)
{
	float current_floor = getFloorPosition(m_stops[m_currentStop]);
	if (m_currentStop >= m_stops.size() - 1) {
		// move backward
		m_nextStop = 0;
		m_target = getFloorPosition(m_stops[0]);
	}
	else {
		// move upward
		m_nextStop = m_currentStop + 1;
		m_target = getFloorPosition(m_stops[m_nextStop]);
	}
	m_direction = (m_target - current_floor) / 100;
}

dfLogicElevator::~dfLogicElevator(void)
{
	for (auto stop : m_stops) {
		delete stop;
	}
}
