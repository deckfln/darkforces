#include "InfElevator.h"

#include "../../config.h"

#include <imgui.h>

#include "../../gaEngine/World.h"
#include "../../gaEngine/gaComponent/gaComponentMesh.h"

#include "../dfComponent.h"
#include "../dfLogicStop.h"
#include "../dfVOC.h"
#include "../dfSector.h"
#include "../dfMesh.h"
#include "../dfSounds.h"
#include "Trigger.h"

#include "../flightRecorder/frCompElevator.h"

/**
 * default elevators speed
 */
static std::map<DarkForces::Component::InfElevator::Type, float> _speeds = {
	{DarkForces::Component::InfElevator::Type::INV, 20.f},
	{DarkForces::Component::InfElevator::Type::BASIC, 20.0f},
	{DarkForces::Component::InfElevator::Type::MOVE_FLOOR, 20.0f},
	{DarkForces::Component::InfElevator::Type::CHANGE_LIGHT, 10.0f},
	{DarkForces::Component::InfElevator::Type::MOVE_CEILING, 20.0f},
	{DarkForces::Component::InfElevator::Type::MORPH_SPIN1, 20.0f},
	{DarkForces::Component::InfElevator::Type::MORPH_SPIN2, 20.0f},
	{DarkForces::Component::InfElevator::Type::MORPH_MOVE1, 20.0f},
	{DarkForces::Component::InfElevator::Type::DOOR, 20.0f}
};

static const std::map<std::string, DarkForces::Keys> g_keys = {
	{ "red", DarkForces::Keys::RED},
	{ "blue", DarkForces::Keys::BLUE},
	{ "yellow", DarkForces::Keys::YELLOW}
};

/**
 * move to the given position (virtual function) => every elevator has its own
 */
void DarkForces::Component::InfElevator::moveTo(float z)
{
	__debugbreak();
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

		// play the starting sound & the moving sound
		m_entity->sendMessage(gaMessage::PLAY_SOUND, DarkForces::Sounds::ELEVATOR_START);
		m_entity->sendMessage(gaMessage::PLAY_SOUND, DarkForces::Sounds::ELEVATOR_MOVE);
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
		return false;

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
					m_entity->sendMessage(gaMessage::STOP_SOUND, DarkForces::Sounds::ELEVATOR_MOVE);
					m_entity->sendMessage(gaMessage::STOP_SOUND, DarkForces::Sounds::ELEVATOR_STOP);
				}
			}
			else {
				// still moving
				// play the end sound if it exists
				m_entity->sendMessage(gaMessage::STOP_SOUND, DarkForces::Sounds::ELEVATOR_MOVE);
				m_entity->sendMessage(gaMessage::PLAY_SOUND, DarkForces::Sounds::ELEVATOR_STOP);

				switch (stop->action()) {
				case dfLogicStop::Action::HOLD:
					m_status = Status::HOLD;
					// stop the animation
					return false;
				case dfLogicStop::Action::TERMINATE:
					m_status = Status::TERMINATED;
					// stop the animation
					return false;
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
	return true;
}

/**
 * Manage the timer
 */
void DarkForces::Component::InfElevator::startTimer(void)
{
	m_tick = 0;
	m_animated = true;
	m_entity->timer(true);
}

void DarkForces::Component::InfElevator::stopTimer(void)
{
	m_animated = false;
	m_entity->timer(false);
}

/**
 * init the object
 */
DarkForces::Component::InfElevator::InfElevator(const std::string& sector, bool smart):
	gaComponent(DF_COMPONENT_INF_ELEVATOR),
	m_sector(sector),
	m_smart(smart)
{
}

/**
 * create from a record
 */
DarkForces::Component::InfElevator::InfElevator(DarkForces::Component::InfElevator::Type kind, dfSector* sector, bool smart):
	gaComponent(DF_COMPONENT_INF_ELEVATOR),
	m_sector(sector->name()),
	m_type(kind),
	m_smart(smart)
{
	m_speed = _speeds[m_type];
	m_entity = sector;
	m_pSector = sector;
}

/**
 * record the maskEvent
 */
void DarkForces::Component::InfElevator::eventMask(uint32_t eventMask)
{
	/* TODO: Hard coded hack for MORPH_SPIN1 & MORPH_MOVE1,
	 * entering the sector doesn't not trigger the elevator to move back to its original position
	 */
	if ((
			m_type == DarkForces::Component::InfElevator::Type::MORPH_SPIN1 || 
			m_type == DarkForces::Component::InfElevator::Type::MORPH_MOVE1
		)
		&& (eventMask & (DarkForces::ENTER_SECTOR | DarkForces::LEAVE_SECTOR))) 
	{
		//entering the sector doesn't not trigger the elevator to move back to its original position
		eventMask &= ~(DarkForces::ENTER_SECTOR | DarkForces::LEAVE_SECTOR);
	}
	m_eventMask = eventMask;
}

/**
 * Record data needed to build a mesh
 */
void DarkForces::Component::InfElevator::meshData(float bottom, float top, uint32_t texture, bool clockwise, dfWallFlag whatToDraw)
{
	m_meshData = true;
	m_meshBottom = bottom;				// lower z value
	m_meshCeiling = top;				// upper z value
	m_meshTexture = texture;			// texture to use
	m_meshClockwise = clockwise;		// draw walls clockwise or not
	m_meshFlag = whatToDraw;			// what walls to draw
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
 * // register a sound for a SART, MOVE, STOP
 */
void DarkForces::Component::InfElevator::addSound(uint32_t action, dfVOC* sound)
{
	__debugbreak();
}

//*******************************************************

/**
 * Force an elevator to go to a specific Stop
 */
void DarkForces::Component::InfElevator::onGotoStopForced(gaMessage* message)
{

	m_currentStop = message->m_value;
	moveTo(m_stops[m_currentStop]);
}

/**
 * activate the elevator
 */
void DarkForces::Component::InfElevator::onTrigger(gaMessage* message)
{
	if (m_status != Status::HOLD) {
		// break the animation and move directly to the next stop
		moveToNextStop();
		m_status = Status::MOVE;
		// no need for animation, there is already one on the message queue
	}
	else {
		// for speed = 0, move instantly to the next stop
		if (m_speed == 0) {
			gaDebugLog(1, "DarkForces::Component::Elevator::dispatchMessage", "speed==0 not implemented");
		}
		else {
			startTimer();
			if (!animate(0)) {
				stopTimer();
			}
		}
	}

	// synchronize all triggers to the status of the elevator
	for (auto trigger : m_triggers) {
		m_entity->sendMessage(trigger->entity()->name(), DarkForces::Message::TRIGGER);
	}
}

/**
 * Move to the next stop
 */
void DarkForces::Component::InfElevator::onGotoStop(gaMessage* message)
{
	if (m_status == Status::HOLD && m_currentStop == message->m_value) {
		return;				// nothing to do, we're already at the correct stop
	}

	if (m_status == Status::MOVE && m_nextStop == message->m_value) {
		return;				// nothing to do, we're already moving toward the requested stop
	}

	m_nextStop = message->m_value;

	if (m_speed > 0) {
		// animated move
		m_current = m_stops[m_currentStop]->z_position(m_type);
		m_target = m_stops[m_nextStop]->z_position(m_type);

		float t1 = m_stops[m_currentStop]->time();
		float t2 = m_stops[m_nextStop]->time();

		float delta = (t2 - t1) * 1000;	// time in millisecond

		m_direction = m_target - m_current;

		// TODO adapt the speed
		m_delay = abs(m_direction) * 1600 / m_speed;

		// only trigger a loop if the object is currently still
		if (m_status == Status::HOLD) {

			// start the sounds
			m_entity->sendMessage(gaMessage::PLAY_SOUND, DarkForces::Sounds::ELEVATOR_START);
			m_entity->sendMessage(gaMessage::PLAY_SOUND, DarkForces::Sounds::ELEVATOR_MOVE);

			m_status = Status::MOVE;
			startTimer();
			if (!animate(0)) {
				stopTimer();
			}
		}
	}
	else {
		// instant move
		m_currentStop = message->m_value;
		moveTo(m_stops[m_currentStop]);
	}
}

/**
 * animate the elevator
 */
void DarkForces::Component::InfElevator::onTimer(gaMessage* message)
{
	if (!animate(message->m_delta)) {
		stopTimer();
	}
}

//*******************************************************

/**
 * Handle messages
 */
void DarkForces::Component::InfElevator::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::TRIGGER:
		onTrigger(message);
		break;

	case DarkForces::Message::GOTO_STOP_FORCE:
		onGotoStopForced(message);
		break;

	case DarkForces::Message::GOTO_STOP:
		onGotoStop(message);
		break;

	case gaMessage::TIMER:
		onTimer(message);
		break;
	}
}

/**
 * Create a ComponentMesh for the elevator
 */
dfMesh* DarkForces::Component::InfElevator::buildMesh(void)
{
	GameEngine::ComponentMesh* component = nullptr;
	dfMesh* mesh;
	std::list<dfLogicTrigger*> signs;				// signs bound to the mesh

	if (!m_pSector) {
		return nullptr;
	}

	//
	// Build a mesh depending of the type
	//
	if (!m_meshData) {
		return nullptr;
	}
	mesh = m_pSector->buildElevator_new(m_meshBottom, m_meshCeiling, m_meshTexture, m_meshClockwise, m_meshFlag, signs);

	// translate the vertices to the center of the elevator
	relocateMesh(mesh);

	// record in the entity
	return mesh;
}

/**
 * register the needed key
 */
void DarkForces::Component::InfElevator::key(const std::string& key)
{
	for (auto& k : g_keys) {
		if (k.first == key) {
			m_key = k.second;
		}
	}
}

/**
 * set the mesh data before the final build
 */

void DarkForces::Component::InfElevator::prepareMesh(void)
{
	if (m_zmin == INFINITY) {
		m_zmin = m_pSector->staticFloorAltitude();
		m_zmax = m_pSector->staticCeilingAltitude();
	}
}

//*******************************************************

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

//*******************************************************

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
	static std::map<DarkForces::Component::InfElevator::Type, const char *>  types = {
		{DarkForces::Component::InfElevator::Type::INV, "inv"},
		{DarkForces::Component::InfElevator::Type::BASIC, "basic"},
		{DarkForces::Component::InfElevator::Type::MOVE_FLOOR, "move_floor"},
		{DarkForces::Component::InfElevator::Type::CHANGE_LIGHT, "change_light"},
		{DarkForces::Component::InfElevator::Type::MOVE_CEILING, "move_ceiling"},
		{DarkForces::Component::InfElevator::Type::MORPH_SPIN1, "morph_spin1"},
		{DarkForces::Component::InfElevator::Type::MORPH_MOVE1, "morph_move1"},
		{DarkForces::Component::InfElevator::Type::MORPH_SPIN2, "morph_spin2"},
		{DarkForces::Component::InfElevator::Type::DOOR, "door" }
	};

	if (ImGui::TreeNode("InfElevator")) {
		ImGui::Text("Type:%s", types[m_type]);
		ImGui::Text("Stops:%d", m_stops.size());
		ImGui::Text("Status : %s", status[m_status]);
		if (m_status == Status::MOVE) {
			ImGui::Text("Tick: %.2f", m_tick);
			ImGui::Text("Delay: %.2f", m_delay);
			ImGui::Text("Stops: current:%d next:%d", m_currentStop, m_nextStop);
			ImGui::Text("Z: current:%.4f target:%.2f", m_current, m_target);
			ImGui::Text("Speed: %.2f", m_direction);
		}
		else {
			ImGui::Text("Stop: %d", m_currentStop);
			ImGui::Text("Z: %.2f", m_current);
		}
		ImGui::TreePop();
	}
}
