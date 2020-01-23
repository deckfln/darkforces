#include "dfLogicElevator.h"

#define _USE_MATH_DEFINES // for C++
#include <math.h>

#include "dfMesh.h"
#include "dfSector.h"
#include "dfLevel.h"
#include "dfSign.h"

const std::list<std::string> keywords = {
	"inv",			//DF_ELEVATOR_INV
	"basic",		//DF_ELEVATOR_BASIC
	"move_floor",	//DF_ELEVATOR_MOVE_FLOOR
	"change_light",	//DF_ELEVATOR_CHANGE_LIGHT
	"move_ceiling",	//DF_ELEVATOR_MOVE_CEILING
	"morph_spin1"	//DF_ELEVATOR_MORPH_SPIN1
};

dfLogicElevator::dfLogicElevator(std::string& kind, dfSector* sector, dfLevel* parent):
	m_class(kind),
	m_sector(sector->m_name),
	m_pSector(sector),
	m_parent(parent)
{
	unsigned int i = 0;
	for (auto &keyword : keywords) {
		if (keyword == kind) {
			m_type = i;
			return;
		}
		i++;
	}

	std::cerr << "dfLogicElevator::dfLogicElevator " << kind << " not implemented" << std::endl;
}

dfLogicElevator::dfLogicElevator(std::string& kind, std::string& sector):
	m_class(kind),
	m_sector(sector)
{
	unsigned int i = 0;
	for (auto& keyword : keywords) {
		if (keyword == kind) {
			m_type = i;
			return;
		}
		i++;
	}

	std::cerr << "dfLogicElevator::dfLogicElevator " << kind << " not implemented" << std::endl;
}

/**
 * bind the evelator to its sector
 * for any relative stop, record the floor
 */
void dfLogicElevator::bindSector(dfSector* pSector)
{
	static std::string standard = "standard";

	m_pSector = pSector;
	for (auto stop : m_stops) {
		stop->sector(pSector);
	}

	// if the elevator has mask_event for enter/leave, create triggers
	if (m_eventMask & DF_ELEVATOR_ENTER_SECTOR) {
		m_pSector->addTrigger(DF_ELEVATOR_ENTER_SECTOR);
	}
	if (m_eventMask & DF_ELEVATOR_LEAVE_SECTOR) {
		m_pSector->addTrigger(DF_ELEVATOR_LEAVE_SECTOR);
	}

	// get the maximum extend of the elevator 
	float amin = 99999, amax = -99999, c;
	for (auto stop : m_stops) {
		c = stop->z_position();
		if (c < amin) amin = c;
		if (c > amax) amax = c;
	}

	switch (m_type) {
	case DF_ELEVATOR_INV:
	case DF_ELEVATOR_MOVE_FLOOR:
		m_pSector->m_floorAltitude = amin;
		break;
	
	case DF_ELEVATOR_MOVE_CEILING:
	case DF_ELEVATOR_BASIC:
		m_pSector->m_ceilingAltitude = amax;
		break;

	case DF_ELEVATOR_MORPH_SPIN1:
		// remove all non-portal walls. These walls will be stored on the Elevator mesh
		m_pSector->removeHollowWalls();
		break;
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
		if (m_type == DF_ELEVATOR_INV) {
			m_pSector->buildElevator(m_mesh, 0, amax - amin, DFWALL_TEXTURE_TOP, true, -1);
		}
		else {
			m_pSector->buildElevator(m_mesh, 0, -(amax - amin), DFWALL_TEXTURE_TOP, true, -1);
		}

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
	case DF_ELEVATOR_MOVE_CEILING:
		m_mesh = new dfMesh(material);

		if (m_type == DF_ELEVATOR_MOVE_FLOOR) {
			// the elevator top is actually the floor
			m_pSector->buildElevator(m_mesh, -(amax - amin), 0, DFWALL_TEXTURE_BOTTOM, false, -1);
		}
		else {
			// move ceiling, only move the top
			m_pSector->buildElevator(m_mesh, 0, (amax - amin), DFWALL_TEXTURE_TOP, false, -1);
		}

		if (m_mesh->buildMesh()) {
			m_mesh->mesh()->set_name(m_pSector->m_name);
			m_pSector->addObject(m_mesh->mesh());
		}
		else {
			// do not keep the trigger
			delete m_mesh;
			m_mesh = nullptr;
			return nullptr;
		}
		break;

	case DF_ELEVATOR_MORPH_SPIN1:
		m_mesh = new dfMesh(material);

		// only use the inner polygon (the hole)
		m_pSector->buildElevator(m_mesh, m_pSector->m_floorAltitude, m_pSector->m_ceilingAltitude, DFWALL_TEXTURE_MID, false, 2);

		// move the vertices around the center (in level space)
		m_center.z = m_pSector->m_floorAltitude;
		m_mesh->moveVertices(m_center);

		if (m_mesh->buildMesh()) {
			m_mesh->mesh()->set_name(m_pSector->m_name);
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
	m_currentStop = stopID;
	dfLogicStop* stop = m_stops[m_currentStop];

	if (this->m_sector == "elev3-5") {
		std::cerr << "dfLogicElevator::init ignored elevator elev3-5" << std::endl;
		return;
	}

	if (m_mesh) {
		moveTo(stop);
	}

	// send messages to the clients
	stop->sendMessages();
}

/**
 * compute the move to the next Stop
 */
void dfLogicElevator::moveToNextStop(void)
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
	m_delay = abs(m_direction) * 838 / m_speed;
}

/**
 * Move the object on the 2 axise
 */
bool dfLogicElevator::animateMoveZ(void)
{
	switch (m_status) {
	case DF_ELEVATOR_HOLD:
		m_status = DF_ELEVATOR_MOVE;
		m_tick = 0;
		moveToNextStop();
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
			dfLogicStop* stop;

			m_currentStop = m_nextStop;
			stop = m_stops[m_currentStop];

			// force the altitude to get ride of math round
			moveTo(stop);

			// send messages to the clients
			stop->sendMessages();

			if (stop->isTimeBased()) {
				// put the elevator on wait
				m_status = DF_ELEVATOR_WAIT;
			}
			else {
				std::string& action = stop->action();
				if (action == "hold") {
					m_status = DF_ELEVATOR_HOLD;

					// stop the animation
					return true;	
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
			moveToNextStop();
			m_status = DF_ELEVATOR_MOVE;
			m_tick = 0;
		}
		break;

	default:
		std::cerr << "dfLogicElevator::animate unknown status " << m_status << std::endl;
	}

	return false;
}

void dfLogicElevator::setSignsStatus(int status)
{
	for (auto sign : m_signs) {
		sign->setStatus(status);	// the elevator is open'
	}
}

/**
 * move through the animations of the elevator
 */
bool dfLogicElevator::animate(time_t delta)
{
	m_tick += delta;

	switch (m_type) {
	case DF_ELEVATOR_BASIC:
	case DF_ELEVATOR_INV:
	case DF_ELEVATOR_MOVE_FLOOR:
	case DF_ELEVATOR_MOVE_CEILING:
	case DF_ELEVATOR_MORPH_SPIN1:
		return animateMoveZ();
	}

	return true;	// Animation is not implemented, stop it
}

/**
 * Compute a floor altitude based on elevator kind and stop
 */
void dfLogicElevator::moveTo(dfLogicStop *stop)
{
	float z = stop->z_position();
	moveTo(z);
}

/**
 * Compute a floor altitude based on elevator kind and stop
 */
void dfLogicElevator::moveTo(float z)
{
	if (m_mesh == nullptr) {
		std::cerr << "dfLogicElevator::moveTo mesh not implemented for " << m_sector << std::endl;
		return;
	}

	switch (m_type) {
	case DF_ELEVATOR_INV:
		m_mesh->moveCeilingTo(z);
		break;
	case DF_ELEVATOR_BASIC:
		m_mesh->moveCeilingTo(z);
		break;
	case DF_ELEVATOR_MOVE_FLOOR:
		m_mesh->moveFloorTo(z);
		m_pSector->floor(z);
		break;
	case DF_ELEVATOR_MOVE_CEILING:
		m_mesh->moveCeilingTo(z);
		m_pSector->ceiling(z);
		break;
	case DF_ELEVATOR_MORPH_SPIN1:
		m_mesh->rotateZ(glm::radians((z)));
		break;
	default:
		std::cerr << "dfLogicElevator::moveTo m_type==" << m_type << " not implemented" << std::endl;
	}
}

/**
 * For every stop of the elevator, bind the messages to the elevators
 */
void dfLogicElevator::bindStopMessage2Elevator(std::map<std::string, dfLogicElevator*>& hashElevators)
{
	for (auto stop : m_stops) {
		stop->bindMessage2Elevator(hashElevators);
	}
}

/**
 * Records signs that trigger the evelvator
 */
void dfLogicElevator::addSign(dfSign* sign)
{
	m_signs.push_back(sign);
}

/**
 * Dispatch and handle messages
 */
void dfLogicElevator::dispatchMessage(dfMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_TRIGGER:
		if (m_status != DF_ELEVATOR_HOLD) {
			// break the animation and move directly to the next stop
			moveToNextStop();
			m_status = DF_ELEVATOR_MOVE;
			m_tick = 0;
		}
		else {
			// for speed = 0, move instantly to the next stop
			if (m_speed == 0) {
				std::cerr << "dfLogicElevator::trigger speed==0 not implemented" << std::endl;
			}

			// start the animation
			m_parent->activateElevator(this);
			animate(0);
		}
		break;

	case DF_MESSAGE_GOTO_STOP:
		if (m_currentStop == message->m_value) {
			return;	// nothing to do, we're at the right floor
		}
		m_nextStop = message->m_value;

		if (m_speed > 0) {
			// animated move
			m_current = m_stops[m_currentStop]->z_position();
			m_target = m_stops[m_nextStop]->z_position();

			float t1 = m_stops[m_currentStop]->time();
			float t2 = m_stops[m_nextStop]->time();

			float delta = (t2 - t1) * 1000;	// time in milisecond

			m_direction = m_target - m_current;

			// TODO adapt the speed
			m_delay = abs(m_direction) * 838 / m_speed;

			m_status = DF_ELEVATOR_MOVE;
			m_tick = 0;
			m_parent->activateElevator(this);
		}
		else {
			// instant move
			m_currentStop = message->m_value;
			moveTo(m_stops[m_currentStop]);
		}
		break;

	default:
		std::cerr << "dfLogicElevator::dispatchMessage message " << message->m_action << " not implemented" << std::endl;
	}
}

dfLogicElevator::~dfLogicElevator(void)
{
	for (auto stop : m_stops) {
		delete stop;
	}
}
