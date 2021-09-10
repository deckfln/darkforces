#include "gaMessage.h"

#include <iostream>
#include <imgui.h>
#include <map>

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

gaMessage::gaMessage(const std::string& server, const std::string& client, int action, const glm::vec3& value, void* extra):
    m_server(server),
    m_client(client),
    m_action(action),
    m_value(0),
    m_v3value(value),
    m_extra(extra)
{
}

/**
 * duplicate a message
 */
gaMessage::gaMessage(gaMessage* source)
{
    m_client = source->m_client;
    m_server = source->m_server;
    m_action = source->m_action;
    m_delta = source->m_delta;
    m_fvalue = source->m_fvalue;
    m_v3value = source->m_v3value;
    m_value = source->m_value;

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

/**
 * debugger
 */
static std::map<int32_t, const char*> g_definitions = {
    { gaMessage::Action::COLLIDE, "COLLIDE"},
    { gaMessage::Action::TIMER, "TIMER"},
    { gaMessage::Action::DELETE_ENTITY, "DELETE_ENTITY"},
    { gaMessage::Action::MOVE, "MOVE"},
    { gaMessage::Action::ROTATE, "ROTATE"},
    { gaMessage::Action::PLAY_SOUND, "PLAY_SOUND"},
    { gaMessage::Action::STOP_SOUND, "STOP_SOUND"},
    { gaMessage::Action::WANT_TO_MOVE, "WANT_TO_MOVE"},
    { gaMessage::Action::COLLISION, "COLLISION"},
    { gaMessage::Action::MOVE_TO, "MOVE_TO"},
    { gaMessage::Action::WORLD_INSERT,"WORLD_INSERT" },
    { gaMessage::Action::WORLD_REMOVE, "WORLD_REMOVE"},
    { gaMessage::Action::WOULD_FALL, "WOULD_FALL"},
    { gaMessage::Action::FALL, "FALL"},
    { gaMessage::Action::CONTROLLER, "CONTROLLER"},
    { gaMessage::Action::SAVE_WORLD, "SAVE_WORLD"},
    { gaMessage::Action::KEY, "KEY"}
};

static std::map<int32_t, std::map<int, const char*>> g_definitions_values = {
};

void gaMessage::debugGUI(void)
{

    ImGui::TableNextColumn();
    ImGui::Text(m_server.c_str());
    ImGui::TableNextColumn();
    ImGui::Text(m_client.c_str());
    ImGui::TableNextColumn();

    if (g_definitions.count(m_action) == 0) {
        ImGui::Text("***%d***", m_action);
    }
    else {
        ImGui::Text(g_definitions[m_action]);
    }
    ImGui::TableNextColumn();

    if (g_definitions_values.count(m_action) > 0) {
        auto& values = g_definitions_values[m_action];

        if (values.count(m_value) > 0) {
            const char* txt = values[m_value];
            ImGui::Text(txt);
        }
        else {
            ImGui::Text("%d", m_value);
        }
    }
    else {
        ImGui::Text("%d", m_value);
    }
    ImGui::TableNextColumn();
    ImGui::Text("%.2f", m_fvalue);
    ImGui::TableNextRow();
}

/**
 * Add new messages definition for the debugger
 */
void gaMessage::declareMessages(const std::map<int, const char*>& actions, const std::map<int32_t, const std::map<int32_t, const char*>>& values)
{
    for (auto& action : actions) {
        g_definitions[action.first] = action.second;
    }

    for (auto value : values) {
        g_definitions_values[value.first] = value.second;
    }
}
