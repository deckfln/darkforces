#include "dfMessageClient.h"

#include "dfMessageBus.h"

dfMessageClient::dfMessageClient(std::string& name) :
	m_name(name)
{
	g_MessageBus.addClient(this);
}

void dfMessageClient::addToBus(void)
{
	g_MessageBus.addClient(this);
}

dfMessageClient::~dfMessageClient()
{
	g_MessageBus.removeClient(this);
}
