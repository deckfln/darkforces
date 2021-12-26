#include "dfTrackPlayer.h"

#include <tinyxml2.h>

#include "../dfComponent/dfEnemyAI.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

/**
 *
 */
void DarkForces::Behavior::TrackPlayer::onChildExit(uint32_t child, Status status)
{
	// remove programmed alarm
	g_gaWorld.cancelAlarmEvent(m_alarmID);

	// check the player location, but only react to visibility, not to distance from last knwon position because we are away
	bool* b = m_tree->blackboard<bool>("player_visible");
	if (b == nullptr || *b == false) {
		// we still can' see the player, so the tracking failed
		m_children[m_runningChild]->status(Status::FAILED);
	}
	else {
		m_children[m_runningChild]->status(Status::SUCCESSED);
	}
}

/**
 *
 */
DarkForces::Behavior::TrackPlayer::TrackPlayer(const char* name):
	GameEngine::Behavior::Decorator(name)
{
}

BehaviorNode* DarkForces::Behavior::TrackPlayer::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::TrackPlayer* node = new DarkForces::Behavior::TrackPlayer(name);
	tinyxml2::XMLElement* walk = element->FirstChildElement("walk");
	if (walk) {
		int mmin = 2000;
		int mmax = 2000;
		const char* rnd = nullptr;
		bool brnd = false;

		walk->QueryIntAttribute("min", &mmin);
		walk->QueryIntAttribute("max", &mmax);
		rnd = walk->Attribute("random");
		if (rnd != nullptr) {
			if (strcmp(rnd, "true") == 0) {
				brnd = true;
			}
		}

		node->m_maximum_walk = mmax;
		node->m_minimum_walk = mmin;
		node->m_random = brnd;

	}
	return node;
}

/**
 *
 */
void DarkForces::Behavior::TrackPlayer::init(void* data)
{
	/*
	static int pi = 0;
	if (m_entity->name() == "OFFCFIN.WAX(21)") {
		pi++;
		if (pi == 2) {
			m_tree->blackboard<bool>("debug_satnave", true);
		}
	}
	*/
	// Pick the last 2 known positions and run the entity along the axe up to a wall
	std::deque<glm::vec3>* playerLastPositions = m_tree->blackboard<std::deque<glm::vec3>>("player_last_positions");
	size_t size = playerLastPositions->size();
	glm::vec2 direction;

	if (size < 2) {
		// if we don't have enough position of the player, were and when did we last heard a blaster shot

		glm::vec3* sound = m_tree->blackboard<glm::vec3>("last_heard_sound");
		if (sound == nullptr) {
			m_status = Status::FAILED;
			return;
		}

		m_target = glm::vec3(sound->x, m_entity->position().y, sound->z);
	}
	else {
		// the player may have been seen twice at the same position, so find a different position, but only go back a bit
		glm::vec3 p1 = playerLastPositions->at(--size);
		glm::vec3 p2 = playerLastPositions->at(--size);
		int i = 5;

		while (p1.x == p2.x && p1.z == p2.z && size > 0) {
			p2 = playerLastPositions->at(--size);
		}
		if (size <= 0) {
			// player as static all the time
			m_target = playerLastPositions->back();
		}
		else {
			glm::vec2 p1d(p1.x, p1.z);
			glm::vec2 p2d(p2.x, p2.z);

#ifdef _DEBUG
			if (p1d == p2d) {
				__debugbreak();
			}
#endif

			direction = glm::normalize(p1d - p2d);
			m_target = glm::vec3(direction.x + p1d.x, m_entity->position().y, direction.y + p1d.y);
		}

	}

	if (m_random) {
		m_walk = (rand() % (m_maximum_walk - m_minimum_walk)) + m_minimum_walk;
	}
	else {
		m_walk = m_maximum_walk;
	}

	// walk only for 2s
	GameEngine::Alarm alarm(m_entity, m_walk, gaMessage::Action::SatNav_CANCEL);
	m_alarmID = g_gaWorld.registerAlarmEvent(alarm);

	GameEngine::BehaviorNode::init(&m_target);
}

//-------------------------------------------

/**
 * display the component in the debugger
 */
void DarkForces::Behavior::TrackPlayer::debugGUInode(void)
{
	ImGui::Text("%.2f %.2f %.2f", m_target.x, m_target.y, m_target.z);
	ImGui::Text("max_walk:%d", m_walk);
}
