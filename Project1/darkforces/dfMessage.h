#pragma once

#include <string>
#include <vector>
#include <queue>

enum {
	DF_MESSAGE_TRIGGER,
	DF_MESSAGE_GOTO_STOP,
	DF_MESSAGE_DONE,
	DF_MESSAGE_TIMER
};

class dfMessageClient;

class dfMessage {
	dfMessageClient* m_pClient = nullptr;	// cached value

public:
	int m_action = -1;
	int m_value = 0;
	std::string m_client;
	time_t m_delta = 0;	// time since the last frame

	dfMessage(void);
	dfMessage(int action);
	dfMessage(int action, int value);
	dfMessage(int action, int value, std::string& client);
	dfMessage(std::vector<std::string>& tokens);
};