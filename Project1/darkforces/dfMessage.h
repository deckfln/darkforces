#pragma once

#include <string>
#include <vector>
#include <queue>

enum {
	DF_MESSAGE_TRIGGER,
	DF_MESSAGE_GOTO_STOP,
	DF_MESSAGE_DONE
};

class dfLogicElevator;
class dfSign;

class dfMessage {
public:
	int m_action = -1;
	int m_value = 0;
	std::string m_client;

	dfLogicElevator* m_pClient = nullptr;	// GOTO on an elevator
	dfSign* m_pSign = nullptr;				// DONE on a switch

	dfMessage(void);
	dfMessage(int action);
	dfMessage(int action, int value);
	dfMessage(int action, int value, std::string& client);
	dfMessage(std::vector<std::string>& tokens);
};