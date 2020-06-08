#include "dfLogicElevator.h"

#define _USE_MATH_DEFINES // for C++
#include <math.h>

#include "dfMesh.h"
#include "dfSector.h"
#include "dfLevel.h"
#include "dfSign.h"
#include "dfMessageBus.h"

static std::map<std::string, dfElevatorType>  keywords = {
	{"inv",			dfElevatorType::INV},
	{"basic",		dfElevatorType::BASIC},
	{"move_floor",	dfElevatorType::MOVE_FLOOR},
	{"change_light",dfElevatorType::CHANGE_LIGHT},
	{"move_ceiling",dfElevatorType::MOVE_CEILING},
	{"morph_spin1",	dfElevatorType::MORPH_SPIN1},
	{"morph_move1",	dfElevatorType::MORPH_MOVE1},
	{"morph_spin2",	dfElevatorType::MORPH_SPIN2}
};

// default elevators speed
static std::map<dfElevatorType, float> _speeds = {
	{dfElevatorType::INV, 20.f},
	{dfElevatorType::BASIC, 20.0f},	
	{dfElevatorType::MOVE_FLOOR, 20.0f},
	{dfElevatorType::CHANGE_LIGHT, 10.0f},
	{dfElevatorType::MOVE_CEILING, 20.0f},
	{dfElevatorType::MORPH_SPIN1, 20.0f},
	{dfElevatorType::MORPH_MOVE1, 20.0f}
};

dfLogicElevator::dfLogicElevator(std::string& kind, dfSector* sector, dfLevel* parent):
	dfMessageClient(sector->m_name),
	m_class(kind),
	m_sector(sector->m_name),
	m_pSector(sector),
	m_parent(parent)
{
	m_msg_animate.m_client = m_name;

	if (keywords.count(kind) > 0) {
		m_type = keywords[kind];
		m_speed = _speeds[m_type];
	}
	else {
		std::cerr << "dfLogicElevator::dfLogicElevator " << kind << " not implemented" << std::endl;
	}
}

dfLogicElevator::dfLogicElevator(std::string& kind, std::string& sector):
	dfMessageClient(sector),
	m_class(kind),
	m_sector(sector)
{
	m_msg_animate.m_client = m_name;

	if (keywords.count(kind) > 0) {
		m_type = keywords[kind];
		m_speed = _speeds[m_type];
	}
	else {
		std::cerr << "dfLogicElevator::dfLogicElevator " << kind << " not implemented" << std::endl;
	}
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

	// inform the sector it is driven by an elevator
	m_pSector->elevator(this);

	// get the maximum extend of the elevator 
	float amin = 99999, amax = -99999, c;
	for (auto stop : m_stops) {
		c = stop->z_position(m_type);
		if (c < amin) amin = c;
		if (c > amax) amax = c;
	}

	switch (m_type) {
	case dfElevatorType::INV:
		m_pSector->staticCeilingAltitude(amax);
		break;

	case dfElevatorType::MOVE_FLOOR:
		m_pSector->staticFloorAltitude(amin);
		break;
	
	case dfElevatorType::BASIC:
		m_pSector->staticCeilingAltitude(amax);
		break;

	case dfElevatorType::MOVE_CEILING:
		m_pSector->staticCeilingAltitude(amax);
		m_pSector->ceiling( m_pSector->referenceFloor() );
		break;

	case dfElevatorType::MORPH_SPIN1:
	case dfElevatorType::MORPH_MOVE1:
	case dfElevatorType::MORPH_SPIN2:
		// remove all non-portal walls. These walls will be stored on the Elevator mesh
		m_pSector->removeHollowWalls();

		// if the sector is included in another one (eg slider_sw on secbase)
		// remove the hollows from the parent sector
		dfSector* parent = m_pSector->isIncludedIn();
		if (parent != nullptr) {
			parent->removeHollowWalls();
		}
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
	case dfElevatorType::INV:
	case dfElevatorType::BASIC:
	case dfElevatorType::MOVE_FLOOR:
	case dfElevatorType::MOVE_CEILING:
		// only vertical moving elevator needs to be tested against the stop
		float c;
		for (auto stop : m_stops) {
			c = stop->z_position(m_type);
			if (c < m_zmin) m_zmin = c;
			if (c > m_zmax) m_zmax = c;
		}
		break;
	default:
		m_zmin = m_pSector->staticFloorAltitude();
		m_zmax = m_pSector->staticCeilingAltitude();
	}


	switch (m_type) {
	case dfElevatorType::INV:
	case dfElevatorType::BASIC:
		m_mesh = new dfMesh(material, bitmaps);

		// the elevator bottom is actually the ceiling
		if (m_type == dfElevatorType::INV) {
			m_pSector->buildElevator(m_mesh, 0, m_zmax - m_zmin, DFWALL_TEXTURE_TOP, true, dfWallFlag::ALL);
			m_pSector->setAABBtop(m_zmax);
		}
		else {
			m_pSector->buildElevator(m_mesh, 0, -(m_zmax - m_zmin), DFWALL_TEXTURE_TOP, true, dfWallFlag::ALL);
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

	case dfElevatorType::MOVE_FLOOR:
	case dfElevatorType::MOVE_CEILING:
		m_mesh = new dfMesh(material, bitmaps);

		if (m_type == dfElevatorType::MOVE_FLOOR) {
			// the elevator top is actually the floor
			m_pSector->buildElevator(m_mesh, -(m_zmax - m_zmin), 0, DFWALL_TEXTURE_BOTTOM, false, dfWallFlag::ALL);
			m_pSector->setAABBbottom(m_zmin);
		}
		else {
			// move ceiling, only move the top
			m_pSector->buildElevator(m_mesh, 0, (m_zmax - m_zmin), DFWALL_TEXTURE_TOP, false, dfWallFlag::ALL);
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

	case dfElevatorType::MORPH_SPIN1:
	case dfElevatorType::MORPH_MOVE1:
	case dfElevatorType::MORPH_SPIN2:
		m_mesh = new dfMesh(material, bitmaps);

		// only use the inner polygon (the hole)
		// these elevators are always portal, 
		// textures to use and the height are based on the difference between the connected sectors floor & ceiling and the current floor & ceiling
		m_pSector->buildElevator(m_mesh, m_zmin, m_zmax, DFWALL_TEXTURE_MID, false, dfWallFlag::MORPHS_WITH_ELEV);

		switch (m_type) {
		case dfElevatorType::MORPH_SPIN1:
		case dfElevatorType::MORPH_SPIN2:
			// move the vertices around the center (in level space)
			m_center.z = m_pSector->referenceFloor();
			m_mesh->moveVertices(m_center);
			break;
		case dfElevatorType::MORPH_MOVE1:
			// elevator moves along the m_move vector
			m_mesh->findCenter();
			break;
		default:
			std::cerr << "dfLogicElevator::buildGeometry m_type=" << int(m_type) << " unsupported" << std::endl;
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

	if (m_type == dfElevatorType::CHANGE_LIGHT) {
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
	case dfElevatorStatus::TERMINATED:
		// the elevator cannot be moved
		return true;

	case dfElevatorStatus::HOLD:
		m_status = dfElevatorStatus::MOVE;
		m_tick = 0;

		// play the starting sound if it exists
		if (m_mesh && m_sounds[dfElevatorSound::START] != nullptr) {
			m_mesh->play(m_sounds[dfElevatorSound::START]);
		}
		moveToNextStop();
		break;

	case dfElevatorStatus::MOVE: {
		if (m_mesh && !m_mesh->play()) {
			// play the moving sound if it exists
			// but only after the starting sound has ended
			if (m_sounds[dfElevatorSound::MOVE] != nullptr) {
				m_mesh->play(m_sounds[dfElevatorSound::MOVE]);
			}
		}

		if (m_direction != 0) {
			m_current = m_target - m_direction * (1.0f - m_tick / m_delay);
		}
		else {
			m_current = m_target;
		}
		moveTo(m_current);

		bool reached = false;
		if (m_direction < 0) {
			reached = m_target >= m_current;
		}
		else {
			reached = m_current >= m_target;
		}

		if (reached) {
			// play the end sound if it exists
			if (m_mesh && m_sounds[dfElevatorSound::END] != nullptr) {
				m_mesh->play(m_sounds[dfElevatorSound::END]);
			}

			dfLogicStop* stop;

			m_currentStop = m_nextStop;
			stop = m_stops[m_currentStop];

			// force the altitude to get ride of math round
			moveTo(stop);

			// send messages to the clients
			stop->sendMessages();

			if (stop->isTimeBased()) {
				// put the elevator on wait
				m_status = dfElevatorStatus::WAIT;
			}
			else {
				switch (stop->action()) {
				case DF_STOP_HOLD:
						m_status = dfElevatorStatus::HOLD;
						// stop the animation
						return true;
				case DF_STOP_TERMINATE:
					m_status = dfElevatorStatus::TERMINATED;
					// stop the animation
					return true;
				default:
					std::cerr << "dfLogicElevator::animate action " << stop->action() << " not implemented" << std::endl;
				}
			}
		}
		break;
	}

	case dfElevatorStatus::WAIT:
		if (m_tick >= m_stops[m_currentStop]->time()) {
			moveToNextStop();
			m_status = dfElevatorStatus::MOVE;
			m_tick = 0;
		}
		break;

	default:
		std::cerr << "dfLogicElevator::animate unknown status " << int(m_status) << std::endl;
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
	case dfElevatorType::CHANGE_LIGHT:
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
	case dfElevatorType::BASIC:
	case dfElevatorType::INV:
	case dfElevatorType::MOVE_FLOOR:
	case dfElevatorType::MOVE_CEILING:
	case dfElevatorType::MORPH_SPIN1:
	case dfElevatorType::MORPH_MOVE1:
	case dfElevatorType::MORPH_SPIN2:
		return animateMoveZ();
	default:
		std::cerr << "dfLogicElevator::animate m_type=" << int(m_type) << " not implemented" << std::endl;
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
	case dfElevatorType::INV:
	case dfElevatorType::BASIC:
	case dfElevatorType::MOVE_FLOOR:
	case dfElevatorType::MOVE_CEILING:
	case dfElevatorType::MORPH_SPIN1:
	case dfElevatorType::MORPH_MOVE1:
	case dfElevatorType::MORPH_SPIN2:
		if (m_mesh == nullptr) {
			//std::cerr << "dfLogicElevator::moveTo mesh not implemented for " << m_sector << std::endl;
			return;
		}
		break;
	case dfElevatorType::CHANGE_LIGHT:
		if (m_pSector == nullptr) {
			//std::cerr << "dfLogicElevator::moveTo sector not found " << m_sector << std::endl;
			return;
		}
		break;
	}

	// run the move
	switch (m_type) {
	case dfElevatorType::INV:
		m_mesh->moveCeilingTo(z);
		break;
	case dfElevatorType::BASIC:
		m_mesh->moveCeilingTo(z);
		break;
	case dfElevatorType::MOVE_FLOOR:
		m_mesh->moveFloorTo(z);
		m_pSector->currentFloorAltitude(z);
		break;
	case dfElevatorType::MOVE_CEILING:
		m_mesh->moveCeilingTo(z);
		m_pSector->ceiling(z);
		break;
	case dfElevatorType::MORPH_SPIN1:
	case dfElevatorType::MORPH_SPIN2:
		m_mesh->rotateZ(glm::radians((z)));
		break;
	case dfElevatorType::MORPH_MOVE1:
		m_mesh->translate(m_move, z);
		break;
	case dfElevatorType::CHANGE_LIGHT:
		m_pSector->changeAmbient(z);
		break;
	default:
		std::cerr << "dfLogicElevator::moveTo m_type==" << int(m_type) << " not implemented" << std::endl;
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
		std::cerr << "dfLogicElevator::dispatchMessage TRIGGER sector=" << m_name << " action=" << message->m_action << " status=" << int(m_status) << std::endl;;

		if (m_status != dfElevatorStatus::HOLD) {
			// break the animation and move directly to the next stop
			moveToNextStop();
			m_status = dfElevatorStatus::MOVE;
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
		std::cerr << "dfLogicElevator::dispatchMessage GOTO_STOP sector=" << m_name << " action=" << message->m_action << " status=" << int(m_status) << std::endl;;
		if (m_type == dfElevatorType::MORPH_SPIN1 && m_status != dfElevatorStatus::HOLD) {
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
			m_delay = abs(m_direction) * 1600 / m_speed;

			m_status = dfElevatorStatus::MOVE;
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

	// let the parent class deal with the message
	dfMessageClient::dispatchMessage(message);
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
 * Register the sound of the elevator
 */
void dfLogicElevator::sound(int effect, dfVOC* sound)
{
	if (effect < 0 || effect > 3) {
		std::cerr << "dfLogicElevator::sound incorrect sound number" << std::endl;
		return;
	}

	m_sounds[effect] = sound;
}

/**
 * for morph_move1, convert angle translation to a vector translation
 */
void dfLogicElevator::angle(float angle)
{
	angle = glm::radians(angle);	// conver degrees to radians
	m_move = glm::vec3(-sin(angle), cos(angle), 0);
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
