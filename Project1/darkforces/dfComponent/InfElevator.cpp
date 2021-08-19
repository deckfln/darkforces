#include "InfElevator.h"

#include "../../config.h"

#include <imgui.h>

#include "../../gaEngine/World.h"
#include "../../gaEngine/gaComponent/gaComponentMesh.h"

#include "../dfLogicStop.h"
#include "../dfVOC.h"
#include "../dfSector.h"
#include "../dfMesh.h"

#include "../flightRecorder/frCompElevator.h"

/**
 * move to the given position (virtual function) => every elevator has its own
 */
void DarkForces::Component::InfElevator::moveTo(float z)
{
	gaDebugLog(1, "DarkForces::Component::Elevator::moveTo", "m_type==" + std::to_string((uint32_t)m_type) + " not implemented");
}

/**
 * move directly to the given stop
 */
void DarkForces::Component::InfElevator::moveTo(dfLogicStop* stop)
{
	float p = stop->z_position(m_type);
	moveTo(p);
}

/**
 * start moving to the next stop
 */
void DarkForces::Component::InfElevator::moveToNextStop(void)
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
		dfVOC* voc = m_sounds[dfElevator::Sound::START];
		if (voc != nullptr) {
			m_entity->sendInternalMessage(gaMessage::PLAY_SOUND, 0, voc->sound());
		}
		// play the moving sound if it exists
		voc = m_sounds[dfElevator::Sound::MOVE];
		if (voc != nullptr) {
			m_entity->sendInternalMessage(gaMessage::PLAY_SOUND, 0, voc->sound());
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
 * move between stops
 */
bool DarkForces::Component::InfElevator::animate(time_t delta)
{
	m_tick += delta;

	switch (m_status) {
	case Status::TERMINATED:
		// the elevator cannot be moved
		return true;

	case Status::HOLD:
		m_status = Status::MOVE;
		m_tick = 0;

		moveToNextStop();
		break;

	case Status::MOVE: {
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
				m_status = Status::WAIT;

				// stop the move sound and play the end sound if it exists AND the stop is NOT zero
				if (stop->time() != 0) {
					dfVOC* voc = m_sounds[dfElevator::Sound::MOVE];
					if (voc != nullptr) {
						m_entity->sendInternalMessage(gaMessage::STOP_SOUND, 0, voc->sound());
					}
					voc = m_sounds[dfElevator::Sound::END];
					if (voc != nullptr) {
						m_entity->sendInternalMessage(gaMessage::STOP_SOUND, 0, voc->sound());
					}
				}
			}
			else {
				// still moving
				// 
				// play the end sound if it exists
				dfVOC* voc = m_sounds[dfElevator::Sound::MOVE];
				if (voc != nullptr) {
					m_entity->sendInternalMessage(gaMessage::STOP_SOUND, 0, voc->sound());
				}

				voc = m_sounds[dfElevator::Sound::END];
				if (voc != nullptr) {
					m_entity->sendInternalMessage(gaMessage::PLAY_SOUND, 0, voc->sound());
				}

				switch (stop->action()) {
				case dfLogicStop::Action::HOLD:
					m_status = Status::HOLD;
					// stop the animation
					return true;
				case dfLogicStop::Action::TERMINATE:
					m_status = Status::TERMINATED;
					// stop the animation
					return true;
				default:
					gaDebugLog(1, "DarkForces::Component::Elevator::animate", "action " + std::to_string(static_cast<uint32_t>(stop->action())));
				}
			}
		}
		break;
	}

	case Status::WAIT:
		if (m_tick >= m_stops[m_currentStop]->time()) {
			moveToNextStop();
			m_status = Status::MOVE;
			m_tick = 0;
		}
		break;

	default:
		gaDebugLog(1, "DarkForces::Component::Elevator::animate", "action " + std::to_string(static_cast<uint32_t>(m_status)));
	}

	// next animation
	g_gaWorld.sendMessageDelayed(m_entity->name(), m_entity->name(), gaMessage::TIMER, 0, nullptr);
	return false;
}

/**
 * init the object
 */
DarkForces::Component::InfElevator::InfElevator(const std::string& sector):
	gaComponent(DF_COMPONENT_INF_ELEVATOR),
	m_sector(sector)
{
}

/**
 * create from a record
 */
DarkForces::Component::InfElevator::InfElevator(dfSector* sector):
	gaComponent(DF_COMPONENT_INF_ELEVATOR),
	m_sector(sector->name())
{
	m_entity = sector;
	m_pSector = sector;
}

void DarkForces::Component::InfElevator::eventMask(uint32_t eventMask)
{
	/* TODO: Hard coded hack for MORPH_SPIN1,
	 * entering the sector doesn't not trigger the elevator to move back to its original position
	 */
	if (m_type == dfElevator::Type::MORPH_SPIN1 && (eventMask & (DarkForces::ENTER_SECTOR | DarkForces::LEAVE_SECTOR))) {
		eventMask &= ~(DarkForces::ENTER_SECTOR | DarkForces::LEAVE_SECTOR);
	}
	m_eventMask = eventMask;
}

/**
 * add a stop and update the range of the elevator
 */
void DarkForces::Component::InfElevator::addStop(dfLogicStop* stop)
{
	m_stops.push_back(stop);

	float c = stop->z_position(m_type);
	if (c < m_zmin) m_zmin = c;
	if (c > m_zmax) m_zmax = c;
}

/**
 * Handle messages
 */
void DarkForces::Component::InfElevator::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MESSAGE_TRIGGER:
		if (m_status != Status::HOLD) {
			// break the animation and move directly to the next stop
			moveToNextStop();
			m_status = Status::MOVE;
			m_tick = 0;
			// no need for animation, there is already one on the message queue
		}
		else {
			// for speed = 0, move instantly to the next stop
			if (m_speed == 0) {
				gaDebugLog(1, "DarkForces::Component::Elevator::dispatchMessage", "speed==0 not implemented");
			}
			else {
				animate(0);
			}
		}
		break;

	case DF_MESSAGE_GOTO_STOP:
		if (m_currentStop == message->m_value) {
			return;				// nothing to do, we're already at the correct stop
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

			// only trigger a loop if the object is currently still
			if (m_status == Status::HOLD) {
				m_status = Status::MOVE;
				m_tick = 0;
				animate(0);
			}
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
}

/**
 * Create a ComponentMesh for the elevator
 */
GameEngine::ComponentMesh* DarkForces::Component::InfElevator::buildComponentMesh(void)
{
	GameEngine::ComponentMesh* component = nullptr;
	dfMesh* mesh;
	std::list<dfLogicTrigger*> signs;				// signs bound to the mesh

	if (!m_pSector) {
		return nullptr;
	}

	if (m_zmin == 99999) {
		m_zmin = m_pSector->staticFloorAltitude();
		m_zmax = m_pSector->staticCeilingAltitude();
	}

	//!
	// Build a mesh depending of the type
	//
	switch (m_type) {
	case dfElevator::Type::INV:
	case dfElevator::Type::DOOR:
		// the elevator bottom is actually the ceiling
		mesh = m_pSector->buildElevator_new(0, m_zmax - m_zmin, DFWALL_TEXTURE_TOP, true, dfWallFlag::ALL, signs);
		m_pSector->setAABBtop(m_zmax);
		break;

	case dfElevator::Type::BASIC:
		// the elevator bottom is actually the ceiling
		mesh = m_pSector->buildElevator_new(0, -(m_zmax - m_zmin), DFWALL_TEXTURE_TOP, true, dfWallFlag::ALL, signs);
		m_pSector->setAABBtop(m_zmax);
		break;

	case dfElevator::Type::MOVE_FLOOR:
		// the elevator top is actually the floor
		mesh = m_pSector->buildElevator_new(-(m_zmax - m_zmin), 0, DFWALL_TEXTURE_BOTTOM, false, dfWallFlag::ALL, signs);
		m_pSector->setAABBbottom(m_zmin);
		break;

	case dfElevator::Type::MOVE_CEILING:
		// move ceiling, only move the top
		mesh = m_pSector->buildElevator_new(0, (m_zmax - m_zmin), DFWALL_TEXTURE_TOP, false, dfWallFlag::ALL, signs);
		m_pSector->setAABBtop(m_zmax);
		break;

	case dfElevator::Type::MORPH_SPIN1:
	case dfElevator::Type::MORPH_MOVE1:
	case dfElevator::Type::MORPH_SPIN2:
		// only use the inner polygon (the hole)
		// these elevators are always portal, 
		// textures to use and the height are based on the difference between the connected sectors floor & ceiling and the current floor & ceiling
		mesh = 	m_pSector->buildElevator_new(m_zmin, m_zmax, DFWALL_TEXTURE_MID, false, dfWallFlag::MORPHS_WITH_ELEV, signs);
		break;

	default:
		return nullptr;
	}

	if (mesh == nullptr) {
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
		mesh->centerOnGeometryXZ(m_center);
		break;
	case dfElevator::Type::MORPH_MOVE1:
		// for this elevator, move along an axes from a center, so center on XYZ (in level space)
		mesh->centerOnGeometryXYZ(m_center);
		break;
	case dfElevator::Type::MORPH_SPIN1:
	case dfElevator::Type::MORPH_SPIN2:
		// move the vertices around the center (in level space)
		m_center.z = m_pSector->referenceFloor();
		mesh->moveVertices(m_center);
		break;
	}

	// record in the entity
	return mesh->componentMesh();
}

/**
 * size of the component
 */
inline uint32_t DarkForces::Component::InfElevator::recordSize(void)
{
	return sizeof(flightRecorder::DarkForces::CompElevator);
}

/**
 * save the component state in a record
 */
uint32_t DarkForces::Component::InfElevator::recordState(void* r)
{
	flightRecorder::DarkForces::CompElevator* record = static_cast<flightRecorder::DarkForces::CompElevator*>(r);
	record->size = sizeof(flightRecorder::DarkForces::CompElevator);
	record->id = m_id;
	record->m_status = (uint32_t)m_status;		// status of the elevator
	record->m_tick = m_tick;				// current timer
	record->m_delay = m_delay;				// time to run the elevator
	record->m_currentStop = m_currentStop;	// current stop for the running animation
	record->m_nextStop = m_nextStop;		// target altitude

	record->m_current = m_current;			// current altitude of the part to move (floor or ceiling)
	record->m_direction = m_direction;		// direction and speed of the move
	record->m_target = m_direction;			// target altitude

	return record->size;
}

/**
 * reload a component state from a record
 */
uint32_t DarkForces::Component::InfElevator::loadState(void* r)
{
	flightRecorder::DarkForces::CompElevator* record = (flightRecorder::DarkForces::CompElevator*)r;

	m_status = (Status)record->m_status;	// status of the elevator
	m_tick = record->m_tick;			// current timer
	m_delay = record->m_delay;			// time to run the elevator
	m_currentStop = record->m_currentStop;	// current stop for the running animation
	m_nextStop = record->m_nextStop;		// target altitude

	m_current = record->m_current;		// current altitude of the part to move (floor or ceiling)
	m_direction = record->m_direction;	// direction and speed of the move
	m_target = record->m_direction;		// target altitude

	return record->size;
}

/**
 * display the component in the debugger
 */
void DarkForces::Component::InfElevator::debugGUIinline(void)
{
	static std::map<Status, const char*> status = {
		{Status::HOLD, "HOLD"},
		{Status::MOVE, "MOVE"},
		{Status::TERMINATED, "TERMINATED"},
		{Status::WAIT, "WAIT"}
	};
	static std::map<dfElevator::Type, const char *>  types = {
		{dfElevator::Type::INV, "inv"},
		{dfElevator::Type::BASIC, "basic"},
		{dfElevator::Type::MOVE_FLOOR, "move_floor"},
		{dfElevator::Type::CHANGE_LIGHT, "change_light"},
		{dfElevator::Type::MOVE_CEILING, "move_ceiling"},
		{dfElevator::Type::MORPH_SPIN1, "morph_spin1"},
		{dfElevator::Type::MORPH_MOVE1, "morph_move1"},
		{dfElevator::Type::MORPH_SPIN2, "morph_spin2"},
		{dfElevator::Type::DOOR, "door" }
	};

	if (ImGui::TreeNode("dfElevator")) {
		ImGui::Text("Type:%s", types[m_type]);
		ImGui::Text("Stops:%d", m_stops.size());
		ImGui::Text("Status : %s", status[m_status]);
		ImGui::Text("Tick: %.2f", m_tick);
		ImGui::Text("Delay: %.2f", m_delay);
		ImGui::Text("Stops: current:%d next:%d", m_currentStop, m_nextStop);
		ImGui::Text("Z: current:%.4f target:%.2f", m_current, m_target);
		ImGui::Text("Speed: %.2f", m_direction);
		ImGui::TreePop();
	}
}
