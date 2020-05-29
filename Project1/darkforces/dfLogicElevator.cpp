#include "dfLogicElevator.h"

#define _USE_MATH_DEFINES // for C++
#include <math.h>

#include "dfMesh.h"
#include "dfSector.h"
#include "dfLevel.h"
#include "dfSign.h"
#include "dfMessageBus.h"

const static std::list<std::string> keywords = {
	"inv",			//DF_ELEVATOR_INV
	"basic",		//DF_ELEVATOR_BASIC
	"move_floor",	//DF_ELEVATOR_MOVE_FLOOR
	"change_light",	//DF_ELEVATOR_CHANGE_LIGHT
	"move_ceiling",	//DF_ELEVATOR_MOVE_CEILING
	"morph_spin1",	//DF_ELEVATOR_MORPH_SPIN1
	"morph_move1",	//DF_ELEVATOR_MORPH_MOVE1
	"morph_spin2"	//DF_ELEVATOR_MORPH_SPIN2
};

// default elevators speed
static std::map<int, float> _speeds = {
	{DF_ELEVATOR_INV, 20.f},
	{DF_ELEVATOR_BASIC, 20.0f},	
	{DF_ELEVATOR_MOVE_FLOOR, 20.0f},
	{DF_ELEVATOR_CHANGE_LIGHT, 10.0f},
	{DF_ELEVATOR_MOVE_CEILING, 20.0f},
	{DF_ELEVATOR_MORPH_SPIN1, 20.0f},
	{DF_ELEVATOR_MORPH_MOVE1, 20.0f}
};

dfLogicElevator::dfLogicElevator(std::string& kind, dfSector* sector, dfLevel* parent):
	dfMessageClient(sector->m_name),
	m_class(kind),
	m_sector(sector->m_name),
	m_pSector(sector),
	m_parent(parent)
{
	m_msg_animate.m_client = m_name;

	unsigned int i = 0;
	for (auto &keyword : keywords) {
		if (keyword == kind) {
			m_type = i;
			m_speed = _speeds[m_type];
			return;
		}
		i++;
	}

	std::cerr << "dfLogicElevator::dfLogicElevator " << kind << " not implemented" << std::endl;
}

dfLogicElevator::dfLogicElevator(std::string& kind, std::string& sector):
	dfMessageClient(sector),
	m_class(kind),
	m_sector(sector)
{
	m_msg_animate.m_client = m_name;

	unsigned int i = 0;
	for (auto& keyword : keywords) {
		if (keyword == kind) {
			m_type = i;
			m_speed = _speeds[m_type];
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
	m_pSector->eventMask(m_eventMask);
	
	// get the maximum extend of the elevator 
	float amin = 99999, amax = -99999, c;
	for (auto stop : m_stops) {
		c = stop->z_position(m_type);
		if (c < amin) amin = c;
		if (c > amax) amax = c;
	}

	switch (m_type) {
	case DF_ELEVATOR_INV:
	case DF_ELEVATOR_MOVE_FLOOR:
		m_pSector->staticFloorAltitude(amin);
		break;
	
	case DF_ELEVATOR_BASIC:
		m_pSector->staticCeilingAltitude(amax);
		break;

	case DF_ELEVATOR_MOVE_CEILING:
		m_pSector->staticCeilingAltitude(amax);
		m_pSector->ceiling( m_pSector->referenceFloor() );
		break;

	case DF_ELEVATOR_MORPH_SPIN1:
	case DF_ELEVATOR_MORPH_MOVE1:
	case DF_ELEVATOR_MORPH_SPIN2:
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
dfMesh *dfLogicElevator::buildGeometry(fwMaterial* material, std::vector<dfBitmap*>& bitmaps)
{
	if (!m_pSector) {
		return nullptr;
	}

	// get the maximum extend of the elevator -> will become the height of the object
	switch (m_type) {
	case DF_ELEVATOR_INV:
	case DF_ELEVATOR_BASIC:
	case DF_ELEVATOR_MOVE_FLOOR:
	case DF_ELEVATOR_MOVE_CEILING:
		// only vertical moving elevator needs to be tested against the stop
		float c;
		for (auto stop : m_stops) {
			c = stop->z_position(m_type);
			if (c < m_zmin) m_zmin = c;
			if (c > m_zmax) m_zmax = c;
		}
		break;
	default:
		// sliding or spining elevators
		m_zmin = m_pSector->staticFloorAltitude();
		m_zmax = m_pSector->staticCeilingAltitude();
	}


	switch (m_type) {
	case DF_ELEVATOR_INV:
	case DF_ELEVATOR_BASIC:
		m_mesh = new dfMesh(material, bitmaps);

		// the elevator bottom is actually the ceiling
		if (m_type == DF_ELEVATOR_INV) {
			m_pSector->buildElevator(m_mesh, 0, m_zmax - m_zmin, DFWALL_TEXTURE_TOP, true, DF_WALL_ALL);
			m_pSector->setAABBtop(m_zmax);
		}
		else {
			m_pSector->buildElevator(m_mesh, 0, -(m_zmax - m_zmin), DFWALL_TEXTURE_TOP, true, DF_WALL_ALL);
			m_pSector->setAABBtop(m_zmax);
		}

		if (m_mesh->buildMesh()) {
			m_pSector->addObject(m_mesh);
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
		m_mesh = new dfMesh(material, bitmaps);

		if (m_type == DF_ELEVATOR_MOVE_FLOOR) {
			// the elevator top is actually the floor
			m_pSector->buildElevator(m_mesh, -(m_zmax - m_zmin), 0, DFWALL_TEXTURE_BOTTOM, false, DF_WALL_ALL);
			m_pSector->setAABBbottom(m_zmin);
		}
		else {
			// move ceiling, only move the top
			m_pSector->buildElevator(m_mesh, 0, (m_zmax - m_zmin), DFWALL_TEXTURE_TOP, false, DF_WALL_ALL);
			m_pSector->setAABBtop(m_zmax);
		}

		if (m_mesh->buildMesh()) {
			m_mesh->mesh()->set_name(m_pSector->m_name);
			m_pSector->addObject(m_mesh);
		}
		else {
			// do not keep the trigger
			delete m_mesh;
			m_mesh = nullptr;
			return nullptr;
		}
		break;

	case DF_ELEVATOR_MORPH_SPIN1:
	case DF_ELEVATOR_MORPH_MOVE1:
	case DF_ELEVATOR_MORPH_SPIN2:
		m_mesh = new dfMesh(material, bitmaps);

		// only use the inner polygon (the hole)
		m_pSector->buildElevator(m_mesh, m_pSector->referenceFloor(), m_pSector->referenceCeiling(), DFWALL_TEXTURE_MID, false, DF_WALL_MORPHS_WITH_ELEV);

		switch (m_type) {
		case DF_ELEVATOR_MORPH_SPIN1:
		case DF_ELEVATOR_MORPH_SPIN2:
			// move the vertices around the center (in level space)
			m_center.z = m_pSector->referenceFloor();
			m_mesh->moveVertices(m_center);
			break;
		case DF_ELEVATOR_MORPH_MOVE1:
			// elevator moves along the m_move vector
			m_mesh->findCenter();
			break;
		default:
			std::cerr << "dfLogicElevator::buildGeometry m_type=" << m_type << " unsupported" << std::endl;
			return nullptr;
		}

		if (m_mesh->buildMesh()) {
			m_mesh->mesh()->set_name(m_pSector->m_name);
			m_pSector->addObject(m_mesh);

			// remove the SPIN1 walls vertices from the sector. otherwise they stay in the way of the collision engine (collision is managed by the dfMesh)
			m_pSector->removeHollowWalls();
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

	return m_mesh;
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

	if (m_type == DF_ELEVATOR_CHANGE_LIGHT) {
		// kick start animation
		g_MessageBus.pushForNextFrame(&m_msg_animate);
	}

	// send messages to the clients
	stop->sendMessages();
}

/**
 * compute the move to the next Stop
 */
void dfLogicElevator::moveToNextStop(void)
{
	m_current = m_stops[m_currentStop]->z_position(m_type);
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

	m_target = m_stops[m_nextStop]->z_position(m_type);
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
	case DF_ELEVATOR_TERMINATED:
		// the elevator cannot be moved
		return true;

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
				switch (stop->action()) {
				case DF_STOP_HOLD:
						m_status = DF_ELEVATOR_HOLD;
						// stop the animation
						return true;
				case DF_STOP_TERMINATE:
					m_status = DF_ELEVATOR_TERMINATED;
					// stop the animation
					return true;
				default:
					std::cerr << "dfLogicElevator::animate action " << stop->action() << " not implemented" << std::endl;
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

	// next animation
	g_MessageBus.pushForNextFrame(&m_msg_animate);
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
	case DF_ELEVATOR_CHANGE_LIGHT:
		if (m_pSector) {
			if (m_pSector->visible()) {
				return animateMoveZ();
			}
			else {
				// next animation
				g_MessageBus.pushForNextFrame(&m_msg_animate);
			}
		}
		break;
	case DF_ELEVATOR_BASIC:
	case DF_ELEVATOR_INV:
	case DF_ELEVATOR_MOVE_FLOOR:
	case DF_ELEVATOR_MOVE_CEILING:
	case DF_ELEVATOR_MORPH_SPIN1:
	case DF_ELEVATOR_MORPH_MOVE1:
	case DF_ELEVATOR_MORPH_SPIN2:
		return animateMoveZ();
	default:
		std::cerr << "dfLogicElevator::animate m_type=" << m_type << " not implemented" << std::endl;
	}

	return true;	// Animation is not implemented, stop it
}

/**
 * Compute a floor altitude based on elevator kind and stop
 */
void dfLogicElevator::moveTo(dfLogicStop *stop)
{
	float z = stop->z_position(m_type);
	moveTo(z);
}

/**
 * Compute a floor altitude based on elevator kind and stop
 */
void dfLogicElevator::moveTo(float z)
{
	// security check
	switch (m_type) {
	case DF_ELEVATOR_INV:
	case DF_ELEVATOR_BASIC:
	case DF_ELEVATOR_MOVE_FLOOR:
	case DF_ELEVATOR_MOVE_CEILING:
	case DF_ELEVATOR_MORPH_SPIN1:
	case DF_ELEVATOR_MORPH_MOVE1:
	case DF_ELEVATOR_MORPH_SPIN2:
		if (m_mesh == nullptr) {
			//std::cerr << "dfLogicElevator::moveTo mesh not implemented for " << m_sector << std::endl;
			return;
		}
		break;
	case DF_ELEVATOR_CHANGE_LIGHT:
		if (m_pSector == nullptr) {
			//std::cerr << "dfLogicElevator::moveTo sector not found " << m_sector << std::endl;
			return;
		}
		break;
	}

	// run the move
	switch (m_type) {
	case DF_ELEVATOR_INV:
		m_mesh->moveCeilingTo(z);
		break;
	case DF_ELEVATOR_BASIC:
		m_mesh->moveCeilingTo(z);
		break;
	case DF_ELEVATOR_MOVE_FLOOR:
		m_mesh->moveFloorTo(z);
		m_pSector->currentFloorAltitude(z);
		break;
	case DF_ELEVATOR_MOVE_CEILING:
		m_mesh->moveCeilingTo(z);
		m_pSector->ceiling(z);
		break;
	case DF_ELEVATOR_MORPH_SPIN1:
	case DF_ELEVATOR_MORPH_SPIN2:
		m_mesh->rotateZ(glm::radians((z)));
		break;
	case DF_ELEVATOR_MORPH_MOVE1:
		m_mesh->translate(m_move, z);
		break;
	case DF_ELEVATOR_CHANGE_LIGHT:
		m_pSector->changeAmbient(z);
		break;
	default:
		std::cerr << "dfLogicElevator::moveTo m_type==" << m_type << " not implemented" << std::endl;
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
		std::cerr << "dfLogicElevator::dispatchMessage TRIGGER sector=" << m_name << " action=" << message->m_action << " status=" << m_status << std::endl;;

		if (m_status != DF_ELEVATOR_HOLD) {
			// break the animation and move directly to the next stop
			moveToNextStop();
			m_status = DF_ELEVATOR_MOVE;
			m_tick = 0;
			// no need for animation, there is already one on the message queue
		}
		else {
			// for speed = 0, move instantly to the next stop
			if (m_speed == 0) {
				std::cerr << "dfLogicElevator::trigger speed==0 not implemented" << std::endl;
			}
			animate(0);
		}
		break;

	case DF_MESSAGE_GOTO_STOP:
		std::cerr << "dfLogicElevator::dispatchMessage GOTO_STOP sector=" << m_name << " action=" << message->m_action << " status=" << m_status << std::endl;;
		if (m_type == DF_ELEVATOR_MORPH_SPIN1 && m_status != DF_ELEVATOR_HOLD) {
			// MORPH_SPIN animation cannot be broken
			return;
		}

		if (m_currentStop == message->m_value) {
			return;	// nothing to do, we're at the right floor
		}
		m_nextStop = message->m_value;

		if (m_speed > 0) {
			// animated move
			m_current = m_stops[m_currentStop]->z_position(m_type);
			m_target = m_stops[m_nextStop]->z_position(m_type);

			float t1 = m_stops[m_currentStop]->time();
			float t2 = m_stops[m_nextStop]->time();

			float delta = (t2 - t1) * 1000;	// time in milisecond

			m_direction = m_target - m_current;

			// TODO adapt the speed
			m_delay = abs(m_direction) * 838 / m_speed;

			m_status = DF_ELEVATOR_MOVE;
			m_tick = 0;
			animate(0);
		}
		else {
			// instant move
			m_currentStop = message->m_value;
			moveTo(m_stops[m_currentStop]);
		}
		break;

	case DF_MESSAGE_TIMER:
		animate(message->m_delta);
		break;

	default:
		std::cerr << "dfLogicElevator::dispatchMessage message " << message->m_action << " not implemented" << std::endl;
	}
}

/**
 * pass the collision detecttion to the mesh
 */
bool dfLogicElevator::checkCollision(float step, glm::vec3& position, glm::vec3& target, float radius, glm::vec3& intersection)
{
	// only test the elevator mesh if the supersector it is bind to is visible
	// and if the play Z (gl space) in inbetwen the vertical elevator extend (level space)
	glm::vec3 plevel;
	m_parent->gl2level(target, plevel);

	float floor, ceiling;

	if (m_pSector) {
		floor = m_pSector->currentFloorAltitude();
		ceiling = m_pSector->ceiling();
	}
	else {
		floor = -1000;
		ceiling = 1000;
	}

	if (m_mesh && m_mesh->visible() && plevel.z > m_zmin && plevel.z < m_zmax && plevel.z < ceiling) {
		return m_mesh->collide(step, position, target, radius, intersection, m_name);
	}

	return false;
}

/**
 * check collision against a fwAABox
 */
bool dfLogicElevator::checkCollision(fwAABBox& box)
{
	// only test the elevator mesh if the supersector it is bind to is visible
	// and if the play Z in inbetwen the vertical elevator extend
	float z = box.m_p.z;

	if (m_mesh && m_mesh->visible() && z > m_zmin && z < m_zmax) {
		return m_mesh->collide(box, m_name);
	}

	return false;
}

/**
 * Get a list of all Sectors that receive messages from that elevator
 */
void dfLogicElevator::getMessagesToSectors(std::list<std::string>& sectors)
{

	for (auto stop : m_stops) {
		stop->getMessagesToSectors(sectors);
	}
}

/**
 * for morph_move1, convert angle translation to a vector translation
 */
void dfLogicElevator::angle(float angle)
{
	angle = glm::radians(angle);	// conver degrees to radians
	m_move = glm::vec3(cos(angle), -sin(angle), 0);
}

/**
 * Return stop number #
 */
dfLogicStop* dfLogicElevator::stop(int i)
{
	if (i < m_stops.size()) {
		return m_stops[i];
	}
	return nullptr;
}

/**
 * Record all keys needed to activate the elevator
 */
const std::string dfKeyRed = "red";

void dfLogicElevator::keys(std::string& key)
{
	if (key == dfKeyRed) {
		m_keys |= DF_KEY_RED;
	}
}

dfLogicElevator::~dfLogicElevator(void)
{
	for (auto stop : m_stops) {
		delete stop;
	}
}
