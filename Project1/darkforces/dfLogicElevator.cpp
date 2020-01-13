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
	else if (kind == "move_floor") {
		m_type = DF_ELEVATOR_MOVE_FLOOR;
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
	else if (kind == "move_floor") {
		m_type = DF_ELEVATOR_MOVE_FLOOR;
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

	// get the maximum extend of the elevator -> will become the height of the object
	float amin = 99999, amax = -99999, c;
	for (auto stop : m_stops) {
		c = stop->z_position();
		if (c < amin) amin = c;
		if (c > amax) amax = c;
	}

	switch (m_type) {
	case DF_ELEVATOR_INV:
	case DF_ELEVATOR_BASIC:
		m_mesh = new dfMesh(material);

		// the elevator bottom is actually the ceiling
		m_pSector->buildElevator(m_mesh, 0, amax - amin, DFWALL_TEXTURE_TOP, true);

		if (m_mesh->buildMesh()) {
			m_pSector->addObject(m_mesh->mesh());
		}
		else {
			// do not keep the trigger
			delete m_mesh;
			m_mesh = nullptr;
			return nullptr;
		}
		break;

	case DF_ELEVATOR_MOVE_FLOOR:
		m_mesh = new dfMesh(material);

		// the elevator top is actually the floor
		m_pSector->buildElevator(m_mesh, -(amax - amin), 0, DFWALL_TEXTURE_BOTTOM, false);

		if (m_mesh->buildMesh()) {
			m_pSector->addObject(m_mesh->mesh());
		}
		else {
			// do not keep the trigger
			delete m_mesh;
			m_mesh = nullptr;
			return nullptr;
		}
		break;

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

	if (m_mesh) {
		moveTo(stop);
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
 * compute the move to the next Stop
 */
void dfLogicElevator::move2nextFloor(void)
{
	m_current = m_stops[m_currentStop]->z_position();
	float t1 = m_stops[m_currentStop]->time();
	float t2;

	if (m_currentStop >= m_stops.size() - 1) {
		// move backward
		m_nextStop = 0;
	}
	else {
		// move upward
		m_nextStop = m_currentStop + 1;
	}

	m_target = m_stops[m_nextStop]->z_position();
	t2 = m_stops[m_nextStop]->time();

	float delta = (t2 - t1) * 1000;	// time in milisecond

	m_direction = m_target - m_current;

	// TODO adapt the speed
	m_delay = abs(m_direction) * 2000 / m_speed;
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
		m_tick = 0;
		move2nextFloor();
		break;

	case DF_ELEVATOR_MOVE: {
		m_current = m_target - m_direction * (1.0f - m_tick / m_delay);
		moveTo(m_current);

		bool reached = false;
		if (m_direction < 0) {
			reached = m_target >= m_current;
		}
		else {
			reached = m_current >= m_target;
		}

		if (reached) {
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
			m_tick = 0;
		}
		break;

	default:
		std::cerr << "dfLogicElevator::animate unknown status " << m_status << std::endl;
	}

	return false;
}

/**
 * update the original sector if the elevator is going to replace moving parts
 */
void dfLogicElevator::updateSectorForMoveFloors(void)
{
	if (!m_pSector) {
		return;
	}

	// get the maximum extend of the elevator -> will become the height of the object
	float amin = 99999, amax = -99999, c;

	for (auto stop : m_stops) {
		c = stop->z_position();
		if (c < amin) amin = c;
		if (c > amax) amax = c;
	}

	m_pSector->m_floorAltitude = amin;
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
		break;
	case DF_ELEVATOR_MOVE_FLOOR:
		m_mesh->moveFloorTo(z);
		m_pSector->floor(z);
		break;
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
	case DF_ELEVATOR_MOVE_FLOOR:
		m_mesh->moveFloorTo(z);
		m_pSector->floor(z);
		break;
	}
}

/**
 * Create a trigger based on the floor of the elevator (for move_floor)
 */
dfLogicTrigger* dfLogicElevator::createFloorTrigger()
{
	static std::string switch1 = "switch1";

	if (!m_pSector) {
		return nullptr;
	}

	dfLogicTrigger* trigger = new dfLogicTrigger(switch1, m_pSector, this);

	return trigger;
}

dfLogicElevator::~dfLogicElevator(void)
{
	for (auto stop : m_stops) {
		delete stop;
	}
}
