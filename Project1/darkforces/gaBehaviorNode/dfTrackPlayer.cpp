#include "dfTrackPlayer.h"

#include <tinyxml2.h>

#include "../dfComponent/dfEnemyAI.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

/**
 *
 */
DarkForces::Behavior::TrackPlayer::TrackPlayer(const char* name):
	GameEngine::Behavior::SetVar(name)
{
}

GameEngine::BehaviorNode* DarkForces::Behavior::TrackPlayer::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::TrackPlayer* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::TrackPlayer*>(p);
	}
	else {
		cl = new DarkForces::Behavior::TrackPlayer(m_name);
	}
	GameEngine::Behavior::SetVar::clone(cl);
	return cl;
}

BehaviorNode* DarkForces::Behavior::TrackPlayer::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::TrackPlayer* node;

	if (used == nullptr) {
		node = new DarkForces::Behavior::TrackPlayer(name);
	}
	else {
		node = dynamic_cast<DarkForces::Behavior::TrackPlayer*>(used);
	}

	GameEngine::Behavior::SetVar::create(name, element, node);
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
	std::deque<glm::vec3>& playerLastPositions = m_tree->blackboard<std::deque<glm::vec3>>("player_last_positions");
	size_t size = playerLastPositions.size();
	glm::vec2 direction;

	if (size < 2) {
		// if we don't have enough position of the player, were and when did we last heard a blaster shot

		glm::vec3& sound = m_tree->blackboard<glm::vec3>("last_heard_sound");
		if (sound == glm::vec3(0)) {
			m_status = Status::FAILED;
			return;
		}

		m_target = glm::vec3(sound.x, m_entity->position().y, sound.z);
	}
	else {
		// the player may have been seen twice at the same position, so find a different position, but only go back a bit
		glm::vec3 p1 = playerLastPositions.at(--size);
		glm::vec3 p2 = playerLastPositions.at(--size);
		int i = 5;

		while (p1.x == p2.x && p1.z == p2.z && size > 0) {
			p2 = playerLastPositions.at(--size);
		}
		if (size <= 0) {
			// player as static all the time
			m_target = playerLastPositions.back();
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

	m_v3value = m_target;
	GameEngine::Behavior::SetVar::init(&m_target);
}
