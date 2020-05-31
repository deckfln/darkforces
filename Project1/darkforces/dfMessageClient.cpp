#include "dfMessageClient.h"

#include "dfMessageBus.h"

dfMessageClient::dfMessageClient(std::string& name) :
	m_name(name)
{
	g_MessageBus.addClient(this);
}

/**
 * Root function to disptach messages to multiple clients
 */
void dfMessageClient::dispatchMessage(dfMessage* message)
{
	if (m_next != nullptr) {
		m_next->dispatchMessage(message);
	}
}

/**
 * Register the client on the message bus
 */
void dfMessageClient::addToBus(void)
{
	g_MessageBus.addClient(this);
}

/**
 * Chain multiple clients
 */
void dfMessageClient::chain(dfMessageClient* next)
{
	m_next = next;
}

dfMessageClient::~dfMessageClient()
{
	g_MessageBus.removeClient(this);
}
