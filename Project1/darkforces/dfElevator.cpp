#include "dfElevator.h"

#define _USE_MATH_DEFINES // for C++
#include <math.h>
#include <imgui.h>

#include "../config.h"

#include "../framework/math/fwCylinder.h"

#include "../gaEngine/World.h"
#include "../gaEngine/gaBoundingBoxes.h"

#include "dfMesh.h"
#include "dfSector.h"
#include "dfLevel.h"
#include "dfSign.h"
#include "dfVOC.h"
#include "dfFileSystem.h"
#include "dfLogicStop.h"

#include "../flightRecorder/frElevator.h"

// elevator categories
static std::map<std::string, dfElevator::Type>  keywords = {
	{"inv",			dfElevator::Type::INV},
	{"basic",		dfElevator::Type::BASIC},
	{"move_floor",	dfElevator::Type::MOVE_FLOOR},
	{"change_light",dfElevator::Type::CHANGE_LIGHT},
	{"move_ceiling",dfElevator::Type::MOVE_CEILING},
	{"morph_spin1",	dfElevator::Type::MORPH_SPIN1},
	{"morph_move1",	dfElevator::Type::MORPH_MOVE1},
	{"morph_spin2",	dfElevator::Type::MORPH_SPIN2},
	{"door",		dfElevator::Type::DOOR }
};

// default elevators speed
static std::map<dfElevator::Type, float> _speeds = {
	{dfElevator::Type::INV, 20.f},
	{dfElevator::Type::BASIC, 20.0f},	
	{dfElevator::Type::MOVE_FLOOR, 20.0f},
	{dfElevator::Type::CHANGE_LIGHT, 10.0f},
	{dfElevator::Type::MOVE_CEILING, 20.0f},
	{dfElevator::Type::MORPH_SPIN1, 20.0f},
	{dfElevator::Type::MORPH_SPIN2, 20.0f},
	{dfElevator::Type::MORPH_MOVE1, 20.0f},
	{dfElevator::Type::DOOR, 20.0f}
};

// default elevators sounds
static std::vector<std::vector<std::string>> g_Default_sounds = {
	{ { "elev2-1.voc", "elev2-2.voc", "elev2-3.voc"} },
	{ { "door2-1.voc", "door2-2.voc", "door2-3.voc"} },
	{ { "door.voc", "", ""} }
};

static std::map<dfElevator::Type, int> g_sound_evelators = {
	{dfElevator::Type::INV, 1},
	{dfElevator::Type::BASIC, 0},
	{dfElevator::Type::MOVE_FLOOR, 0},
	{dfElevator::Type::MOVE_CEILING, 1},
	{dfElevator::Type::MORPH_SPIN1, 1},
	{dfElevator::Type::MORPH_MOVE1, 1},
	{dfElevator::Type::MORPH_SPIN2, 1},
	{dfElevator::Type::DOOR, 2}
};

static std::map<std::string, dfVOC*> m_cachedVOC;

/**
 * Initialize the elevator
 */
void dfElevator::init(const std::string& kind) 
{
	// convert the elevator category from text to bind
	if (keywords.count(kind) > 0) {
		m_type = keywords[kind];
		m_speed = _speeds[m_type];

		// init the default sound
		if (g_sound_evelators.count(m_type) > 0) {
			int sounds = g_sound_evelators[m_type];

			for (int i = 0; i < 3; i++) {
				const std::string& file = g_Default_sounds[sounds][i];
				if (file != "") {
					if (m_cachedVOC.count(file) == 0) {
						m_cachedVOC[file] = new dfVOC(g_dfFiles, file);
					}
					m_sounds[i] = m_cachedVOC[file];
				}
			}
		}
	}
	else {
		std::cerr << "dfElevator::dfElevator " << kind << " not implemented" << std::endl;
	}

	m_physical = true;		// elevators cannot be traversed
	m_gravity = false;		// elevators are not affected by gravity
	m_collideSectors=false;	// elevators doesn't need to check collision with sectors
	m_hasCollider = true;	// test collision with elevators
	m_defaultCollision = gaMessage::Flag::PUSH_ENTITIES;
}

/**
 *
 */
dfElevator::dfElevator(std::string& kind, dfSector* sector, dfLevel* parent):
	gaEntity(DF_ENTITY_ELEVATOR, sector->m_name),
	m_sector(sector->m_name),
	m_pSector(sector),
	m_parent(parent)
{
	init(kind);
}

/**
 *
 */
dfElevator::dfElevator(std::string& kind, std::string& sector):
	gaEntity(DF_ENTITY_ELEVATOR, sector),
	m_sector(sector)
{
	init(kind);
}

/**
 *
 */
dfElevator::dfElevator(dfElevator* source):
	gaEntity((flightRecorder::Entity*)source)
{
	m_status = source->m_status;		// status of the elevator
	m_tick = source->m_tick;			// current timer
	m_delay = source->m_delay;			// time to run the elevator
	m_currentStop = source->m_delay;	// current stop for the running animation
	m_nextStop = source->m_delay;		// target altitude

	m_current = source->m_current;		// current altitude of the part to move (floor or ceiling)
	m_direction = source->m_direction;	// direction and speed of the move
	m_target = source->m_direction;		// target altitude
}

/**
 * bind the elevator to its sector
 * for any relative stop, record the floor
 */
void dfElevator::bindSector(dfSector* pSector)
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
	case dfElevator::Type::INV:
	case dfElevator::Type::DOOR:
		m_pSector->staticCeilingAltitude(amax);
		break;

	case dfElevator::Type::MOVE_FLOOR:
		m_pSector->staticFloorAltitude(amin);
		break;
	
	case dfElevator::Type::BASIC:
		m_pSector->staticCeilingAltitude(amax);
		break;

	case dfElevator::Type::MOVE_CEILING:
		m_pSector->staticCeilingAltitude(amax);
		m_pSector->ceiling( m_pSector->referenceFloor() );
		break;

	case dfElevator::Type::MORPH_SPIN1:
	case dfElevator::Type::MORPH_MOVE1:
	case dfElevator::Type::MORPH_SPIN2:
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
void dfElevator::addStop(dfLogicStop* stop)
{
	m_stops.push_back(stop);
}

/**
 * Create an Mesh for the elevator
 */
dfMesh *dfElevator::buildGeometry(fwMaterial* material, std::vector<dfBitmap*>& bitmaps)
{
	if (!m_pSector) {
		return nullptr;
	}

	//
	// get the maximum extend of the elevator -> will become the height of the object
	//
	switch (m_type) {
	case dfElevator::Type::INV:
	case dfElevator::Type::DOOR:
	case dfElevator::Type::BASIC:
	case dfElevator::Type::MOVE_FLOOR:
	case dfElevator::Type::MOVE_CEILING:
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

	//!
	// Build a mesh depending of the type
	//
	switch (m_type) {
	case dfElevator::Type::INV:
	case dfElevator::Type::DOOR:
	case dfElevator::Type::BASIC:
		m_mesh = new dfMesh(material, bitmaps);

		// the elevator bottom is actually the ceiling
		if (m_type == dfElevator::Type::INV || m_type == dfElevator::Type::DOOR) {
			m_pSector->buildElevator(this, m_mesh, 0, m_zmax - m_zmin, DFWALL_TEXTURE_TOP, true, dfWallFlag::ALL);
			m_pSector->setAABBtop(m_zmax);
		}
		else {
			m_pSector->buildElevator(this, m_mesh, 0, -(m_zmax - m_zmin), DFWALL_TEXTURE_TOP, true, dfWallFlag::ALL);
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

	case dfElevator::Type::MOVE_FLOOR:
	case dfElevator::Type::MOVE_CEILING:
		m_mesh = new dfMesh(material, bitmaps);

		if (m_type == dfElevator::Type::MOVE_FLOOR) {
			// the elevator top is actually the floor
			m_pSector->buildElevator(this, m_mesh, -(m_zmax - m_zmin), 0, DFWALL_TEXTURE_BOTTOM, false, dfWallFlag::ALL);
			m_pSector->setAABBbottom(m_zmin);
		}
		else {
			// move ceiling, only move the top
			m_pSector->buildElevator(this, m_mesh, 0, (m_zmax - m_zmin), DFWALL_TEXTURE_TOP, false, dfWallFlag::ALL);
			m_pSector->setAABBtop(m_zmax);
		}

		if (m_mesh->buildMesh()) {
			m_mesh->name(m_pSector->m_name);
			m_pSector->addObject(m_mesh);
		}
		else {
			// do not keep the trigger
			delete m_mesh;
			m_mesh = nullptr;
			return nullptr;
		}
		break;

	case dfElevator::Type::MORPH_SPIN1:
	case dfElevator::Type::MORPH_MOVE1:
	case dfElevator::Type::MORPH_SPIN2:
		m_mesh = new dfMesh(material, bitmaps);

		// only use the inner polygon (the hole)
		// these elevators are always portal, 
		// textures to use and the height are based on the difference between the connected sectors floor & ceiling and the current floor & ceiling
		m_pSector->buildElevator(this, m_mesh, m_zmin, m_zmax, DFWALL_TEXTURE_MID, false, dfWallFlag::MORPHS_WITH_ELEV);

		if (m_mesh->buildMesh()) {
			m_pSector->addObject(m_mesh);

			// remove the SPIN1 walls vertices from the sector. otherwise they stay in the way of the move engine (move is managed by the dfMesh)
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

	//
	// translate the vertices to the center of the elevator
	//
	switch (m_type) {
	case dfElevator::Type::INV:
	case dfElevator::Type::DOOR:
	case dfElevator::Type::BASIC:
	case dfElevator::Type::MOVE_FLOOR:
	case dfElevator::Type::MOVE_CEILING:
		// for these elevator, Z is defined by the elevator, so center on XY (in level space)
		m_mesh->centerOnGeometryXZ(m_center);
		break;
	case dfElevator::Type::MORPH_MOVE1:
		// for this elevator, move along an axes from a center, so center on XYZ (in level space)
		m_mesh->centerOnGeometryXYZ(m_center);
		break;
	case dfElevator::Type::MORPH_SPIN1:
	case dfElevator::Type::MORPH_SPIN2:
		// move the vertices around the center (in level space)
		m_center.z = m_pSector->referenceFloor();
		m_mesh->moveVertices(m_center);
		break;
	}

	m_mesh->name(m_sector);

	// build the model AABB
	m_modelAABB = m_mesh->modelAABB();
	sendInternalMessage(gaMessage::MOVE, 0, (void*)&m_mesh->position());

	// change the default collider (AABB) to Geometry
	m_collider.set(m_mesh->geometry(), &m_worldMatrix, &m_inverseWorldMatrix);

	// record in the entity
	addComponent(m_mesh->componentMesh());

	return m_mesh;
}

/**
 * move the connected elevator to its HOLD position
 */
void dfElevator::init(int stopID)
{
	m_currentStop = stopID;
	dfLogicStop* stop = m_stops[m_currentStop];

	if (m_mesh) {
		moveTo(stop);
	}

	if (m_type == dfElevator::Type::CHANGE_LIGHT) {
		// kick start animation
		g_gaWorld.sendMessageDelayed(m_name, m_name, gaMessage::TIMER, 0, nullptr);
	}

	// send messages to the clients
	stop->sendMessages();
}

/**
 * compute the move to the next Stop
 */
void dfElevator::moveToNextStop(void)
{
	if (m_speed > 0) {
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

		float delta = (t2 - t1) * 1000;	// time in millisecond

		m_direction = m_target - m_current;

		// TODO adapt the speed
		m_delay = abs(m_direction) * 838 / m_speed;

		// play the starting sound if it exists
		if (m_mesh) {
			dfVOC* voc = m_sounds[dfElevator::Sound::START];
			if (voc != nullptr) {
				sendInternalMessage(gaMessage::PLAY_SOUND, 0, voc->sound());
#ifdef DEBUG
				gaDebugLog(0, "dfElevator::moveToNextStop play", voc->name());
#endif
			}
			// play the moving sound if it exists
			voc = m_sounds[dfElevator::Sound::MOVE];
			if (voc != nullptr) {
				sendInternalMessage(gaMessage::PLAY_SOUND, 0, voc->sound());
#ifdef DEBUG
				gaDebugLog(0, "dfElevator::moveToNextStop play", voc->name());
#endif
			}
		}
	}
	else {
		// instant move
		if (m_currentStop >= m_stops.size() - 1) {
			// move backward
			m_currentStop = 0;
		}
		else {
			// move upward
			m_currentStop = m_currentStop + 1;
		}

		moveTo(m_stops[m_currentStop]);
	}
}

/**
 * Move the object on the 2 axis
 */
bool dfElevator::animateMoveZ(void)
{
	switch (m_status) {
	case dfElevator::Status::TERMINATED:
		// the elevator cannot be moved
		return true;

	case dfElevator::Status::HOLD:
		m_status = dfElevator::Status::MOVE;
		m_tick = 0;

		moveToNextStop();
		break;

	case dfElevator::Status::MOVE: {
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
			dfLogicStop* stop;

			m_currentStop = m_nextStop;
			stop = m_stops[m_currentStop];

			// force the altitude to get ride of math round
			moveTo(stop);

			// send messages to the clients
			stop->sendMessages();

			if (stop->isTimeBased()) {
				// put the elevator on wait
				m_status = dfElevator::Status::WAIT;

				// stop the move sound and play the end sound if it exists AND the stop is NOT zero
				if (stop->time() != 0 && m_mesh) {
					dfVOC* voc = m_sounds[dfElevator::Sound::MOVE];
					if (voc != nullptr) {
						sendInternalMessage(gaMessage::STOP_SOUND, 0, voc->sound());
#ifdef DEBUG
						gaDebugLog(0, "dfElevator::animateMoveZ stop", voc->name());
#endif
					}
					voc = m_sounds[dfElevator::Sound::END];
					if ( voc != nullptr) {
						sendInternalMessage(gaMessage::STOP_SOUND, 0, voc->sound());
#ifdef DEBUG
						gaDebugLog(0, "dfElevator::animateMoveZ stop", voc->name());
#endif
					}
				}
			}
			else {
				// play the end sound if it exists
				if (m_mesh) {
					dfVOC* voc = m_sounds[dfElevator::Sound::MOVE];
					if (voc != nullptr) {
						sendInternalMessage(gaMessage::STOP_SOUND, 0, voc->sound());
#ifdef DEBUG
						gaDebugLog(0, "dfElevator::animateMoveZ stop", voc->name());
#endif
					}

					voc = m_sounds[dfElevator::Sound::END];
					if (voc != nullptr) {
						sendInternalMessage(gaMessage::PLAY_SOUND, 0, voc->sound());
#ifdef DEBUG
						gaDebugLog(0, "dfElevator::animateMoveZ play", voc->name());
#endif
					}
				}

				switch (stop->action()) {
				case dfLogicStop::Action::HOLD:
						m_status = dfElevator::Status::HOLD;
						// stop the animation
						return true;
				case dfLogicStop::Action::TERMINATE:
					m_status = dfElevator::Status::TERMINATED;
					// stop the animation
					return true;
				default:
					std::cerr << "dfElevator::animate action " << static_cast<int>(stop->action()) << " not implemented" << std::endl;
				}
			}
		}
		break;
	}

	case dfElevator::Status::WAIT:
		if (m_tick >= m_stops[m_currentStop]->time()) {
			moveToNextStop();
			m_status = dfElevator::Status::MOVE;
			m_tick = 0;
		}
		break;

	default:
		std::cerr << "dfElevator::animate unknown status " << int(m_status) << std::endl;
	}

	// next animation
	g_gaWorld.sendMessageDelayed(m_name, m_name, gaMessage::TIMER, 0, nullptr);
	return false;
}

void dfElevator::setSignsStatus(int status)
{
	for (auto sign : m_signs) {
		sign->setStatus(status);	// the elevator is open'
	}
}

/**
 * move through the animations of the elevator
 */
bool dfElevator::animate(time_t delta)
{
	m_tick += delta;

	switch (m_type) {
	case dfElevator::Type::CHANGE_LIGHT:
		if (m_pSector) {
			if (m_pSector->visible()) {
				return animateMoveZ();
			}
			else {
				// next animation
				g_gaWorld.sendMessageDelayed(m_name, m_name, gaMessage::TIMER, 0, nullptr);
			}
		}
		break;
	case dfElevator::Type::BASIC:
	case dfElevator::Type::INV:
	case dfElevator::Type::DOOR:
	case dfElevator::Type::MOVE_FLOOR:
	case dfElevator::Type::MOVE_CEILING:
	case dfElevator::Type::MORPH_SPIN1:
	case dfElevator::Type::MORPH_MOVE1:
	case dfElevator::Type::MORPH_SPIN2:
		return animateMoveZ();
	default:
		std::cerr << "dfElevator::animate m_type=" << int(m_type) << " not implemented" << std::endl;
	}

	return true;	// Animation is not implemented, stop it
}

/**
 * Compute a floor altitude based on elevator kind and stop
 */
void dfElevator::moveTo(dfLogicStop *stop)
{
	float z = stop->z_position(m_type);
	moveTo(z);
}

/**
 * Compute a floor altitude based on elevator kind and stop
 * Z is given in level space
 */
void dfElevator::moveTo(float z_lvl)
{
	// security check
	switch (m_type) {
	case dfElevator::Type::INV:
	case dfElevator::Type::DOOR:
	case dfElevator::Type::BASIC:
	case dfElevator::Type::MOVE_FLOOR:
	case dfElevator::Type::MOVE_CEILING:
	case dfElevator::Type::MORPH_SPIN1:
	case dfElevator::Type::MORPH_MOVE1:
	case dfElevator::Type::MORPH_SPIN2:
		if (m_mesh == nullptr) {
			//std::cerr << "dfLogicElevator::moveTo mesh not implemented for " << m_sector << std::endl;
			return;
		}
		break;
	case dfElevator::Type::CHANGE_LIGHT:
		if (m_pSector == nullptr) {
			//std::cerr << "dfLogicElevator::moveTo sector not found " << m_sector << std::endl;
			return;
		}
		break;
	}

	// run the move
	glm::vec3 p = position();
	switch (m_type) {
	case dfElevator::Type::INV:
	case dfElevator::Type::DOOR:
	case dfElevator::Type::BASIC:
	case dfElevator::Type::MOVE_FLOOR:
		// move the sector the elevator is based on (for collision detection)
		m_transforms.m_position = position();
		m_transforms.m_position.y = z_lvl / 10.0f;
		sendMessage(this->m_name, gaMessage::Action::WANT_TO_MOVE, gaMessage::Flag::PUSH_ENTITIES, &m_transforms);
		break;
	case dfElevator::Type::MOVE_CEILING:
		// move the sector the elevator is based on (for collision detection)
		p.y = z_lvl / 10.0f;
		sendInternalMessage(gaMessage::MOVE, 0, &p);
		break;
	case dfElevator::Type::MORPH_SPIN1:
	case dfElevator::Type::MORPH_SPIN2:
		glm::vec3 r = glm::vec3(0, glm::radians(z_lvl), 0);
		sendInternalMessage(gaMessage::ROTATE, gaMessage::Flag::ROTATE_VEC3, &r);
		break;
	case dfElevator::Type::MORPH_MOVE1:
		glm::vec3 p = m_center + m_move * z_lvl;
		dfLevel::level2gl(p);
		sendInternalMessage(gaMessage::MOVE, 0, &p);
		break;
	case dfElevator::Type::CHANGE_LIGHT:
		m_pSector->changeAmbient(z_lvl);
		break;
	default:
		std::cerr << "dfElevator::moveTo m_type==" << int(m_type) << " not implemented" << std::endl;
	}
}

/**
 * Records signs that trigger the elevator
 */
void dfElevator::addSign(dfSign* sign)
{
	m_signs.push_back(sign);
}

/**
 * Dispatch and handle messages
 */
void dfElevator::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_TRIGGER:
		std::cerr << "dfElevator::dispatchMessage TRIGGER sector=" << m_name << " action=" << message->m_action << " status=" << int(m_status) << std::endl;;

		if (m_status != dfElevator::Status::HOLD) {
			// break the animation and move directly to the next stop
			moveToNextStop();
			m_status = dfElevator::Status::MOVE;
			m_tick = 0;
			// no need for animation, there is already one on the message queue
		}
		else {
			// for speed = 0, move instantly to the next stop
			if (m_speed == 0) {
				std::cerr << "dfElevator::trigger speed==0 not implemented" << std::endl;
			}
			animate(0);
		}
		break;

	case DF_MESSAGE_GOTO_STOP:
		std::cerr << "dfElevator::dispatchMessage GOTO_STOP sector=" << m_name << " action=" << message->m_action << " status=" << int(m_status) << std::endl;;
		if (m_type == dfElevator::Type::MORPH_SPIN1 && m_status != dfElevator::Status::HOLD) {
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

			m_status = dfElevator::Status::MOVE;
			m_tick = 0;
			animate(0);
		}
		else {
			// instant move
			m_currentStop = message->m_value;
			moveTo(m_stops[m_currentStop]);
		}
		break;

	case gaMessage::TIMER:
		animate(message->m_delta);
		break;
	}

	gaEntity::dispatchMessage(message);
}

/**
 * Get a list of all Sectors that receive messages from that elevator
 */
void dfElevator::getMessagesToSectors(std::list<std::string>& sectors)
{

	for (auto stop : m_stops) {
		stop->getMessagesToSectors(sectors);
	}
}

/**
 * Register the sound of the elevator
 */
void dfElevator::sound(int effect, dfVOC* sound)
{
	if (effect < 0 || effect > 3) {
		std::cerr << "dfElevator::sound incorrect sound number" << std::endl;
		return;
	}

	m_sounds[effect] = sound;
}

/**
 * for morph_move1, convert angle translation to a vector translation
 */
void dfElevator::angle(float angle)
{
	angle = glm::radians(angle);	// conver degrees to radians
	m_move = glm::vec3(-sin(angle), cos(angle), 0);
}

/**
 * Return stop number #
 */
dfLogicStop* dfElevator::stop(int i)
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

void dfElevator::keys(std::string& key)
{
	if (key == dfKeyRed) {
		m_keys |= DF_KEY_RED;
	}
}

/**
 * return a record of an actor state (for debug)
 */
void dfElevator::recordState(void* r)
{
	gaEntity::recordState(r);
	flightRecorder::Elevator* record = (flightRecorder::Elevator*)r;
	record->entity.classID = flightRecorder::TYPE::DF_ENTITY_ELEVATOR;
	record->entity.size = sizeof(flightRecorder::Elevator);
	record->m_status = (int)m_status;	// status of the elevator
	record->m_tick = m_tick;			// current timer
	record->m_delay = m_delay;			// time to run the elevator
	record->m_currentStop = m_delay;	// current stop for the running animation
	record->m_nextStop = m_delay;		// target altitude

	record->m_current = m_current;		// current altitude of the part to move (floor or ceiling)
	record->m_direction = m_direction;	// direction and speed of the move
	record->m_target = m_direction;		// target altitude

}

/**
 * reload an actor state from a record
 */
void dfElevator::loadState(flightRecorder::Entity* r)
{
	gaEntity::loadState(r);
	flightRecorder::Elevator* record = (flightRecorder::Elevator*)r;

	m_status = (dfElevator::Status)record->m_status;	// status of the elevator
	m_tick = record->m_tick;			// current timer
	m_delay = record->m_delay;			// time to run the elevator
	m_currentStop = record->m_delay;	// current stop for the running animation
	m_nextStop = record->m_delay;		// target altitude

	m_current = record->m_current;		// current altitude of the part to move (floor or ceiling)
	m_direction = record->m_direction;	// direction and speed of the move
	m_target = record->m_direction;		// target altitude
}

/**
 * display inner data to the debugger
 */
void dfElevator::debugGUI(bool* close)
{
	gaEntity::debugGUI(close);

	const char* text = nullptr;

	switch (m_status) {
	case dfElevator::Status::HOLD:
		text = "HOLD";
		break;
	case dfElevator::Status::MOVE:
		text = "MOVE";
		break;
	case dfElevator::Status::TERMINATED:
		text = "TERMINATED";
		break;
	case dfElevator::Status::WAIT:
		text = "WAIT";
		break;
	}

	ImGui::Text("Status : %s", text);
	ImGui::Text("Tick: %.2f", m_tick);
	ImGui::Text("Delay: %.2f", m_delay);
	ImGui::Text("Stops: current:%d next:%d", m_currentStop, m_nextStop);
	ImGui::Text("Z: current:%.2f target:%.2f", m_current, m_target);
	ImGui::Text("Speed: %.2f", m_direction);
}

/**
 *
 */
void* frCreate_Elevator(void* record) {
	return new dfElevator((dfElevator*)record);
}

dfElevator::~dfElevator(void)
{
	for (auto stop : m_stops) {
		delete stop;
	}
}
