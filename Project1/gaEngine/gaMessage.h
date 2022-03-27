#pragma once

#include <string>
#include <vector>
#include <queue>
#include <list>
#include <map>
#include "../framework/fwTransforms.h"
#include "../flightRecorder/Message.h"

class gaEntity;

namespace GameEngine {
	struct Transform : Framework::fwTransforms {
		glm::vec3 m_forward;
		glm::vec3 m_downward;
		int m_flag;
		bool m_rotate=false;

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
		MOVE_ROTATE,	// move and rotatet the entity using a mat4x4
		ROTATE,			// rotate the entity
		REGISTER_SOUND,	// bind a sound to an ID
		PLAY_SOUND,		// play sound bound to ID
		PROPAGATE_SOUND,// propagate a sound in the level
		STOP_SOUND,		// stop sound bound to ID
		WANT_TO_MOVE,	// the entity want to move in a direction (sent to the world)
		COLLISION,		// the entity cannot move, it would collide with ...
		MOVE_TO,		// Move the entity in a direction (m_extra)
		WORLD_INSERT,	// an entity is added to the world
		WORLD_REMOVE,	// an entity is removed from the world
		WOULD_FALL,		// the objects would fall off after a WANT_TO_MOVE
		FALL,			// Object is falling
		CONTROLLER,		// Controller is requesting a move
		SAVE_WORLD,		// save the status
		KEY,			// key is pressed
		KEY_UP,			// key was released
		KEY_DOWN,		// key was pressed
		MOUSE_DOWN,		// mouse button was pressed
		MOUSE_UP,		// mouse button was released
		MOUSE_MOVE,		// mouse moving
		LOOK_AT,		// look into a direction
		MOVE_AT,
		ACTIVATE,		// an active probe tries to activate an entity
		HIDE,			// hide the object on screen
		UNHIDE,			// make the object visible on screen
		START_MOVE,		// the entity starts to move on a path finding
		END_MOVE,		// the entity stops to move on a path finding
		SatNav_GOTO,	// gives a destination to the satnav and start to move
		SatNav_NOGO,	// satnav answers back with no path to reach the destination
		SatNav_CANCEL,	// Cancel the satnav
		SatNav_REACHED,	// satnav reached the destination
		TICK,			// Behavior nodes tick
		ALARM,			// trigger an alarm after Xs
		VIEW,			// an entity entered the cone of vision of an other entity
		NOT_VIEW,		// an entity is not visible
		BULLET_HIT,		// the entity was hit by a bullet
		BULLET_MISS,	// a bullet passed by the entity
		HEAR_SOUND,		// inform a sound is heard
		HEAR_STOP,		// actualy not
		VOLUME_TRANSPARENCY, // change the sound transparency of a volume
		ADD_ITEM,		// add item to the entity inventory
		DROP_ITEM,		// entity shall drop an item
		SCREEN_RESIZE	// provide the screen ration after a resize
	};

	// flags stored in messages
	enum Flag {
		WANT_TO_MOVE_BREAK_IF_FALL,	// Object doesn't want to fall off and want to be informed
		WANT_TO_MOVE_FALL,			// object accepts to fall down
		WANT_TO_MOVE_LASER,			// object is not impacted by gravity
		TRAVERSE_ENTITY,			// traverse a non-physical object
		TRAVERSE_WALL,
		ROTATE_VEC3,
		ROTATE_QUAT,
		ROTATE_BY,
		ROTATE_AXE,
		ROTATE_SPEED
	};

	struct DeclareAction {
		int32_t action;
		const char* title;
	};

	uint32_t m_id=0;				// unique id for the message
	bool m_used = false;
	bool m_canceled = false;

	gaEntity* m_pServer = nullptr;	// cached value

	std::string m_server;			// from
	std::string m_client;			// to
	
	int m_action = -1;
	int m_value = 0;			// int value
	float m_fvalue = 0.0f;			// float value
	glm::vec3 m_v3value = glm::vec3(0.0f); // vec3 value
	void* m_extra = nullptr;		// pointer to extra data
	uint64_t m_data[16];			// store some basic data

	time_t m_delta = 0;				// time since the last frame
	uint32_t m_frame=0;				// current frame the message runs in
	time_t m_time;					// world time

	gaMessage(void);
	gaMessage(int action);
	gaMessage(int action, int value);
	gaMessage(int action, int value, const std::string& client);
	gaMessage(const std::string& server, const std::string& client);
	gaMessage(const std::string& server, const std::string& client, int action, int value, void* extra);
	gaMessage(const std::string& server, const std::string& client, int action, const glm::vec3& value, void* extra=nullptr);
	gaMessage(void *record);
	gaMessage(gaMessage* source);

	void set(const std::string& server, const std::string& client, int action, int value, void* extra);

	const std::string& client(void) { return m_client; };

	int recordSize(void) {
		return sizeof(flightRecorder::Message);
	}													// size of one record
	void recordState(void* record);

	// debugger
	static void debugGUI(flightRecorder::Message*);
	void debugGUI1(void);

	static void declareMessages(const std::map<int, const char*>& actions, const std::map<int32_t, const std::map<int32_t, const char*>>& values);
};