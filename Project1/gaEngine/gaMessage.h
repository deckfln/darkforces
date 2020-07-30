#pragma once

#include <string>
#include <vector>
#include <queue>

enum {
	GA_MSG_COLLIDE = 0,		// two entities checkCollision
	GA_MSG_TIMER,			// animation message
	GA_MSG_DELETE_ENTITY,	// delete the given entity
	GA_MSG_MOVE,			// move the entity to an absolution position
	GA_MSG_ROTATE,			// rotate the entity
	GA_MSG_PLAY_SOUND,
	GA_MSG_STOP_SOUND,
	GA_MSG_WANT_TO_MOVE,	// the entity want to move in a direction (sent to the world)
	GA_MSG_COLLISION,		// the entity cannot move, it would collide with ...
	GA_MSG_MOVE_TO,			// Move the entity in a direction (m_extra)
	GA_MSG_WORLD_INSERT,	// an entity is added to the world
	GA_MSG_WORLD_REMOVE		// an entity is removed from the world
};

class gaEntity;

class gaMessage {

public:
	bool m_used = false;

	gaEntity* m_pServer = nullptr;	// cached value

	std::string m_server;	// from
	std::string m_client;	// to
	
	int m_action = -1;
	int m_value = 0;
	void* m_extra = nullptr;

	time_t m_delta = 0;	// time since the last frame

	gaMessage(void);
	gaMessage(int action);
	gaMessage(int action, int value);
	gaMessage(int action, int value, const std::string& client);
	gaMessage(const std::string& server, const std::string& client);
	gaMessage(const std::string& server, const std::string& client, int action, int value, void* extra);
	void set(const std::string& server, const std::string& client, int action, int value, void* extra);

	const std::string& client(void) { return m_client; };
};