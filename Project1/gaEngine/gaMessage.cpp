#include "gaMessage.h"

#include <iostream>

std::queue<gaMessage*> g_MessagesQueue;

gaMessage::gaMessage(void)
{
};

gaMessage::gaMessage(int action):
    m_action(action)
{
};

gaMessage::gaMessage(int action, int value):
    m_action(action),
    m_value(value)
{
};

gaMessage::gaMessage(int action, int value, const std::string& client):
    m_action(action),
    m_value(value),
    m_client(client)
{
};

gaMessage::gaMessage(const std::string& server, const std::string& client):
    m_server(server),
    m_client(client)
{
};