#pragma once

#include <string>
#include <vector>
#include <queue>

enum {
	GA_MSG_COLLIDE = 0,	// two entities collide
	GA_MSG_TIMER
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
	gaMessage(std::vector<std::string>& tokens);
	std::string& client(void) { return m_client; };
};