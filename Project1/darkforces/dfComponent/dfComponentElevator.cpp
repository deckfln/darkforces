#include "dfComponentElevator.h"

#include "../../config.h"

#include <imgui.h>

#include "../../gaEngine/World.h"

#include "../dfLogicStop.h"
#include "../dfVOC.h"
#include "../dfSector.h"

/**
 * move to the given position (virtual function) => every elevator has its own
 */
void DarkForces::Component::Elevator::moveTo(float z)
{
	gaDebugLog(1, "DarkForces::Component::Elevator::moveTo", "m_type==" + std::to_string((uint32_t)m_type) + " not implemented");
}

/**
 * move directly to the given stop
 */
void DarkForces::Component::Elevator::moveTo(dfLogicStop* stop)
{
	float p = stop->z_position(m_type);
	moveTo(p);
}

/**
 * start moving to the next stop
 */
void DarkForces::Component::Elevator::moveToNextStop(void)
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
bool DarkForces::Component::Elevator::animate(time_t delta)
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
DarkForces::Component::Elevator::Elevator(const std::string& sector):
	gaComponent(DF_COMPONENT_INF_ELEVATOR),
	m_sector(sector)
{
}

DarkForces::Component::Elevator::Elevator(dfSector* sector):
	gaComponent(DF_COMPONENT_INF_ELEVATOR),
	m_sector(sector->name())
{
	m_entity = sector;
	m_pSector = sector;
}

/**
 * Handle messages
 */
void DarkForces::Component::Elevator::dispatchMessage(gaMessage* message)
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
 * display the component in the debugger
 */
void DarkForces::Component::Elevator::debugGUIinline(void)
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
