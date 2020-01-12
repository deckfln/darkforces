#include "dfLogicElevator.h"

#include "dfMesh.h"
#include "dfSector.h"
#include "dfLevel.h"

dfLogicElevator::dfLogicElevator(std::string& kind, dfSector* sector, dfLevel* parent):
	m_class(kind),
	m_pSector(sector),
	m_parent(parent)
{
	if (kind == "inv") {
		m_type = DF_ELEVATOR_INV;
	}
	else if (kind == "basic") {
		m_type = DF_ELEVATOR_BASIC;
	}
	else {
		std::cerr << "dfLogicElevator::dfLogicElevator " << kind << " not implemented" << std::endl;
	}
}

dfLogicElevator::dfLogicElevator(std::string& kind, std::string& sector):
	m_class(kind),
	m_sector(sector)
{
	if (kind == "inv") {
		m_type = DF_ELEVATOR_INV;
	}
	else if (kind == "basic") {
		m_type = DF_ELEVATOR_BASIC;
	}
	else {
		std::cerr << "dfLogicElevator::dfLogicElevator " << kind << " not implemented" << std::endl;
	}
}

/**
 * bind the evelator to its sector
 * for any relative stop, record the floor
 */
void dfLogicElevator::sector(dfSector* pSector)
{
	m_pSector = pSector;
	for (auto stop : m_stops) {
		stop->sector(pSector);
	}
}

/**
 * register the STOP in the correct list
 */
void dfLogicElevator::addStop(dfLogicStop* stop)
{
	m_stops.push_back(stop);
}

/**
 * Create an Mesh for the elevator
 */
fwMesh *dfLogicElevator::buildGeometry(fwMaterial* material)
{
	if (!m_pSector) {
		return nullptr;
	}

	switch (m_type) {
	case DF_ELEVATOR_INV:
	case DF_ELEVATOR_BASIC: {
		// get the maximum extend of the elevator -> will become the height of the object
		float amin = 99999, amax = -99999, c;
		for (auto stop : m_stops) {
			c = stop->z_position();
			if (c < amin) amin = c;
			if (c > amax) amax = c;
		}

		m_mesh = new dfMesh(material);
		m_pSector->buildElevator(m_mesh, 0, amax - amin);

		if (m_mesh->buildMesh()) {
			// there is a mesh
			m_pSector->addObject(m_mesh->mesh());
		}
		break;
	}
	default:
		return nullptr;
	}

	return m_mesh->mesh();
}

/**
 * move the connected elevator to its HOLD position
 */
void dfLogicElevator::init(int stopID)
{
	dfLogicStop* stop = m_stops[stopID];

	if (this->m_sector == "elev3-5") {
		std::cerr << "dfLogicElevator::init ignored elevator elev3-5" << std::endl;
		return;
	}
	moveTo(stop);
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
		m_current += m_direction;
		moveTo(m_current);

		bool reached = false;
		if (m_direction < 0) {
			reached = m_target >= m_current;
		}
		else {
			reached = m_current >= m_target;
		}

		if (reached) {
			m_tick = 0;
			m_currentStop = m_nextStop;

			// force the altitude to get ride of math round
			moveTo(m_stops[m_currentStop]);

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
void dfLogicElevator::moveTo(dfLogicStop *stop)
{
	float z = stop->z_position();

	switch (m_type) {
	case DF_ELEVATOR_INV:
		m_mesh->moveCeilingTo(z);
		break;
	case DF_ELEVATOR_BASIC:
		m_mesh->moveFloorTo(z);
	}
}

/**
 * Compute a floor altitude based on elevator kind and stop
 */
void dfLogicElevator::moveTo(float z)
{
	switch (m_type) {
	case DF_ELEVATOR_INV:
		m_mesh->moveCeilingTo(z);
		break;
	case DF_ELEVATOR_BASIC:
		m_mesh->moveFloorTo(z);
		break;
	}
}

/**
 * compute the move to the next Stop
 */
void dfLogicElevator::move2nextFloor(void)
{
	m_current = m_stops[m_currentStop]->z_position();

	if (m_currentStop >= m_stops.size() - 1) {
		// move backward
		m_nextStop = 0;
		m_target = m_stops[0]->z_position();
	}
	else {
		// move upward
		m_nextStop = m_currentStop + 1;
		m_target = m_stops[m_nextStop]->z_position();
	}
	m_direction = (m_target - m_current) / 100;
}

dfLogicElevator::~dfLogicElevator(void)
{
	for (auto stop : m_stops) {
		delete stop;
	}
}
