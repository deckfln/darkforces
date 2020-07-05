#include "gaMessage.h"

#include <iostream>

std::queue<gaMessage*> g_MessagesQueue;

gaMessage::gaMessage(void)
{
};

gaMessage::gaMessage(int action) 
{
    m_action = action; 
};

gaMessage::gaMessage(int action, int value)
{
    m_action = action; 
    m_value = value; 
};

gaMessage::gaMessage(int action, int value, const std::string& client)
{ 
    m_action = action; 
    m_value = value; 
    m_client = client; 
};

gaMessage::gaMessage(std::vector<std::string>& tokens)
{
	int s = tokens.size();
	const std::string done = "done";
	const std::string gotostop = "goto_stop";

	switch (s) {
	case 5:
		// message: 1 elev3-5 goto_stop 0
		if (tokens[3] == gotostop) {
			m_action = DF_MESSAGE_GOTO_STOP;
			m_value = std::stoi(tokens[4]);
			m_client = tokens[2];
		}
		break;
	case 4:
		// message: 0 ext(6) done
		if (tokens[3] == done) {
			// split sector / wall on tokens[2];
			m_action = DF_MESSAGE_DONE;
			m_value = std::stoi(tokens[1]);
			m_client = tokens[2];
		}
		else if (tokens[3] == "m_trigger") {
			m_action = DF_MESSAGE_TRIGGER;
			m_value = std::stoi(tokens[1]);
			m_client = tokens[2];
		}
		break;
	case 3:
		// message: goto_stop 1
		if (tokens[1] == gotostop) {
			m_action = DF_MESSAGE_GOTO_STOP;
			m_value = std::stoi(tokens[2]);
		}
	}

	// detect failed parsing
	if (m_action == -1) {
		std::string m = "";
		for (auto s : tokens) {
			m += " " + s;
		}
		std::cerr << "gaMessage::parse " << m << " not implemented" << std::endl;
	}
}