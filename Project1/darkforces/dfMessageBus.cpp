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

void dfMessageBus::pushForNextFrame(dfMessage* message)
{
	m_for_next_frame.push(message);
}

void dfMessageBus::process(time_t delta)
{
	dfMessageClient* client;

	while (m_queue.size() > 0) {
		dfMessage* message = m_queue.front();
		m_queue.pop();
		if (m_clients.count(message->m_client) > 0) {
			message->m_delta = delta;
			client = m_clients[message->m_client];
			client->dispatchMessage(message);
		}
	}

	// swap the current queue and the queue for next frame
	m_queue.swap(m_for_next_frame);
}