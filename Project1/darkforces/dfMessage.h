#pragma once

#include <string>

enum {
	DF_MESSAGE_GOTO_STOP
};

const std::string gotostop = "goto_stop";

class dfLogicElevator;

class dfMessage {
public:
	int m_action;
	int m_value;
	std::string m_client;
	dfLogicElevator* m_pClient = nullptr;

	dfMessage(int action, int value) { m_action = action; m_value = value; };
	dfMessage(int action, int value, std::string& client) { m_action = action; m_value = value; m_client = client; };
};
