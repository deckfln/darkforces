#include "gaWorld.h"

#include <iostream>
#include <map>

#include "gaEntity.h"

gaWorld g_gaWorld;

/** 
 * Create and init the world
 */
gaWorld::gaWorld()
{
}

/**
 * Add a new gaEntity
 */
void gaWorld::addClient(gaEntity* client)
{
	m_entities[client->name()].push_back(client);
}

/**
 * remove a gaEntity
 */
void gaWorld::removeClient(gaEntity* client)
{
	if (m_entities.count(client->name()) == 0) {
		// no such client on the map
		return;
	}

	m_entities[client->name()].remove(client);
}

/**
 * Allocate a new message
 */
gaMessage* gaWorld::sendMessage(const std::string& from, const std::string& to, int action, int value, void* extra)
{
	static int lastMessage = 0;
	static gaMessage messages[64];

	// search for an available message
	int count = 64;

	gaMessage* ptr = nullptr;
	do {
		if (--count < 0) {
			assert("not enough messages in gaWorld::getMessage");
		}
		ptr = &messages[lastMessage++];
		if (lastMessage == 64) {
			lastMessage = 0;
		}
	} while (ptr->m_used);

	ptr->m_used = true;

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = value;
	ptr->m_extra = extra;

	m_queue.push(ptr);

	return ptr;
}

/**
 * Search the entities map
 */
gaEntity* gaWorld::getEntity(const std::string& name)
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
void gaWorld::findAABBCollision(fwAABBox& box, std::list<gaEntity*>& collisions)
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

/**
 * extended collision test after a sucessfull AABB collision
 */
bool gaWorld::checkCollision(gaEntity* source, fwCylinder& bounding, glm::vec3& direction, std::list<gaCollisionPoint>& collisions)
{
	glm::vec3 intersection;

	// get all the entities which AABB checkCollision with the player
	fwAABBox aabb(bounding);
	aabb += direction;
	std::list<gaEntity*> entities;
	g_gaWorld.findAABBCollision(aabb, entities);

	for (auto entity : entities) {
		// only test entities that can physically checkCollision, but still inform the target of the collision
		if (!entity->physical()) {
			entity->collideWith(source);
			continue;
		}
		entity->checkCollision(bounding, direction, intersection, collisions);
	}

	return collisions.size() != 0;
}

void gaWorld::push(gaMessage* message)
{
	m_queue.push(message);
}

void gaWorld::pushForNextFrame(gaMessage* message)
{
	m_for_next_frame.push(message);
}

void gaWorld::process(time_t delta)
{
	/*
	if (m_queue.size() > 0) {
		std::cerr << ">>>>>>>>>> gaWorld::process" << std::endl;
	}
	*/

	std::map<std::string, bool> loopDetector;

	while (m_queue.size() > 0) {
		gaMessage* message = m_queue.front();
		m_queue.pop();

		// manage loops inside one run
		if (loopDetector.count(message->m_client) > 0) {
			continue;
		}

		loopDetector[message->m_client] = true;

		if (message->m_action != GA_MSG_TIMER) {
			std::cerr << "gaWorld::process server=" << message->m_server << " action=" << message->m_action << " client=" << message->m_client << std::endl;;
		}

		// animation suspended ?
		if (!m_timer && message->m_action == GA_MSG_TIMER) {
			continue;
		}

		// new clients
		if (m_entities.count(message->m_client) > 0) {
			message->m_delta = delta;
			if (m_entities.count(message->m_server) > 0) {
				message->m_pServer = m_entities[message->m_server].front();
			}
			for (auto entity : m_entities[message->m_client]) {
				entity->dispatchMessage(message);
			}
		}
		message->m_used = false;
	}

	// swap the current queue and the queue for next frame
	m_queue.swap(m_for_next_frame);
}

void gaWorld::suspendTimer(void)
{
	m_timer = false;
}

gaWorld::~gaWorld()
{
}
