#pragma once

#include <queue>
#include <map>

#include "dfMessage.h"
#include "dfMessageClient.h"

class dfMessageBus
{
	std::queue<dfMessage*> m_queue;
	std::map<std::string, dfMessageClient*> m_clients;

public:
	dfMessageBus(void);
	void addClient(dfMessageClient* client);
	void removeClient(dfMessageClient* client);
	dfMessageClient* getClient(std::string& name);
	void push(dfMessage* message);
	void process(void);
};

extern dfMessageBus g_MessageBus;