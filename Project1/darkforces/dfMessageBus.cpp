#include "dfMessagebus.h"

#include <iostream>
#include <map>

dfMessageBus g_MessageBus;

dfMessageBus::dfMessageBus()
{
}

/**
 * Add a new gaEntity
 */
void dfMessageBus::addClient(gaEntity* client)
{
	m_entities[client->name()].push_back(client);
}

/**
 * remove a gaEntity
 */
void dfMessageBus::removeClient(gaEntity* client)
{
	if (m_entities.count(client->name()) == 0) {
		// no such client on the map
		return;
	}

	m_entities[client->name()].remove(client);
}

/**
 * Search the entities map
 */
gaEntity* dfMessageBus::getEntity(const std::string& name)
{
	// search new entities
	if (m_entities.count(name) > 0) {
		return m_entities[name].front();
	}

	return nullptr;
}

/**
 * parse entities to check for collision with the given one
 */
void dfMessageBus::findAABBCollision(fwAABBox& box, std::list<gaEntity*>& collisions)
{
	for (auto entry : m_entities) {
		// test all entity with the same name
		for (auto entity : entry.second) {
			if (entity->collideAABB(box)) {
				collisions.push_back(entity);
			}
		}
	}
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
	/*
	if (m_queue.size() > 0) {
		std::cerr << ">>>>>>>>>> dfMessageBus::process" << std::endl;
	}
	*/

	std::map<std::string, bool> loopDetector;

	while (m_queue.size() > 0) {
		dfMessage* message = m_queue.front();
		m_queue.pop();

		// manage loops inside one run
		if (loopDetector.count(message->m_client) > 0) {
			continue;
		}

		loopDetector[message->m_client] = true;

		if (message->m_action != DF_MESSAGE_TIMER) {
			std::cerr << "dfMessageBus::process server=" << message->m_server << " action=" << message->m_action << " client=" << message->m_client << std::endl;;
		}

		// animation suspended ?
		if (!m_timer && message->m_action == DF_MESSAGE_TIMER) {
			continue;
		}

		// new clients
		if (m_entities.count(message->m_client) > 0) {
			message->m_delta = delta;
			for (auto entity : m_entities[message->m_client]) {
				entity->dispatchMessage(message);
			}
		}
	}

	// swap the current queue and the queue for next frame
	m_queue.swap(m_for_next_frame);
}

void dfMessageBus::suspendTimer(void)
{
	m_timer = false;
}
