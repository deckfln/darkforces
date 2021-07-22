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
}
gaMessage::gaMessage(const std::string& server, const std::string& client, int action, int value, void* extra):
    m_server(server),
    m_client(client),
    m_action(action),
    m_value(value),
    m_extra(extra)
{
}

/**
 * create from the flight recorder
 */
gaMessage::gaMessage(void* r)
{
    flightRecorder::Message* record = (flightRecorder::Message*)r;
    m_client = record->client;
    m_server = record->server;
    m_action = record->action;
    m_delta = record->delta;
    m_fvalue = record->fvalue;
    m_v3value = record->v3value;
    m_value = record->value;
}

void gaMessage::set(const std::string& server, const std::string& client, int action, int value, void* extra)
{
    m_server = server;
    m_client = client;
    m_action = action;
    m_value = value;
    m_extra = extra;
}

/**
 * Save the state of the message
 */
void gaMessage::recordState(void* r)
{
    flightRecorder::Message* record = (flightRecorder::Message * )r;
    strncpy_s(record->client, m_client.c_str(), sizeof(record->client));
    strncpy_s(record->server, m_server.c_str(), sizeof(record->server));
    record->action = m_action;
    record->delta = m_delta;
    record->fvalue = m_fvalue;
    record->v3value = m_v3value;
    record->value = m_value;
}