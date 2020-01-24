#include "dfMessagebus.h"

dfMessageBus g_MessageBus;

dfMessageBus::dfMessageBus()
{
}

void dfMessageBus::addClient(dfMessageClient* client)
{
	m_clients[client->name()] = client;
}

void dfMessageBus::removeClient(dfMessageClient* client)
{
	m_clients.erase(client->name());
}

dfMessageClient* dfMessageBus::getClient(std::string& name)
{
	if (m_clients.count(name) > 0) {
		return m_clients[name];
	}

	return nullptr;
}

void dfMessageBus::push(dfMessage* message)
{
	m_queue.push(message);
}

void dfMessageBus::process(void)
{
	dfMessageClient* client;

	while (m_queue.size() > 0) {
		dfMessage* message = m_queue.front();
		m_queue.pop();
		if (m_clients.count(message->m_client) > 0) {
			client = m_clients[message->m_client];
			client->dispatchMessage(message);
		}
	}
}
