#pragma once

#include <string>
#include <vector>
#include <queue>
#include "../framework/fwTransforms.h"
#include "../flightRecorder/Message.h"

class gaEntity;

namespace GameEngine {
	struct Transform : Framework::fwTransforms {
		glm::vec3 m_forward;
		glm::vec3 m_downward;
		int m_flag;

		void recordState(Transform* record) {
			Framework::fwTransforms::recordState((Framework::fwTransforms*)record);
			record->m_forward = m_forward;
			record->m_downward = m_downward;
			record->m_flag = m_flag;
		}

		void loadState(Transform* record) {
			Framework::fwTransforms::loadState((Framework::fwTransforms*)record);
			m_forward = record->m_forward;
			m_downward = record->m_downward;
			m_flag = record->m_flag;
		}
	};
}

class gaMessage {

public:
	enum Action {
		COLLIDE = 0,	// entity collides with another one
		TIMER,			// animation message
		DELETE_ENTITY,	// delete the given entity
		MOVE,			// move the entity to an absolution position
		ROTATE,			// rotate the entity
		PLAY_SOUND,
		STOP_SOUND,
		WANT_TO_MOVE,	// the entity want to move in a direction (sent to the world)
		COLLISION,		// the entity cannot move, it would collide with ...
		MOVE_TO,		// Move the entity in a direction (m_extra)
		WORLD_INSERT,	// an entity is added to the world
		WORLD_REMOVE,	// an entity is removed from the world
		WOULD_FALL,		// the objects would fall off after a WANT_TO_MOVE
		FALL,			// Object is falling
		CONTROLLER,		// Controller is requesting a move
		SAVE_WORLD		// save the status
	};
	// flags stored in messages
	enum Flag {
		WANT_TO_MOVE_BREAK_IF_FALL,	// Object doesn't want to fall off and want to be informed
		WANT_TO_MOVE_FALL,			// object accepts to fall down
		WANT_TO_MOVE_LASER,			// object is not impacted by gravity
		PUSH_ENTITIES,				// push objects we are colliding with
		COLLIDE_ENTITY,				// collide with a hard physical object
		COLLIDE_WALL,
		TRAVERSE_ENTITY,			// traverse a non-physical object
		TRAVERSE_WALL,
		ROTATE_VEC3,
		ROTATE_QUAT,
		ROTATE_BY,
	};

	bool m_used = false;

	gaEntity* m_pServer = nullptr;	// cached value

	std::string m_server;	// from
	std::string m_client;	// to
	
	int m_action = -1;
	int m_value = 0;
	float m_fvalue = 0.0f;
	glm::vec3 m_v3value = glm::vec3(0.0f);
	void* m_extra = nullptr;

	time_t m_delta = 0;	// time since the last frame
	uint32_t m_frame=0;	// current frame the message runs in

	gaMessage(void);
	gaMessage(int action);
	gaMessage(int action, int value);
	gaMessage(int action, int value, const std::string& client);
	gaMessage(const std::string& server, const std::string& client);
	gaMessage(const std::string& server, const std::string& client, int action, int value, void* extra);
	gaMessage(void *record);
	gaMessage(gaMessage* source);

	void set(const std::string& server, const std::string& client, int action, int value, void* extra);

	const std::string& client(void) { return m_client; };

	int recordSize(void) {
		return sizeof(flightRecorder::Message);
	}													// size of one record
	void recordState(void* record);
};