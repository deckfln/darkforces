#include "gaWorld.h"

#include <iostream>
#include <map>

#include "../framework/fwScene.h"
#include "../framework/fwMesh.h"

#include "gaEntity.h"

#include "../darkforces/dfSuperSector.h"
#include "../darkforces/dfSprites.h"

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
	client->OnWorldInsert();
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

	// remove from the list
	m_entities[client->name()].remove(client);

	// if the list is empty remove the name
	if (m_entities[client->name()].size() == 0) {
		m_entities.erase(client->name());
	}
}

/**
 * add a mesh to the current scene
 */
void gaWorld::add2scene(gaEntity* client)
{
	client->add2scene(m_scene);
}

/**
 * add a game sector
 */
void gaWorld::addSector(dfSuperSector* client)
{
	m_sectors.push_back(client);
}

/**
 * add a new model to the world
 */
void gaWorld::addModel(GameEngine::gaModel* model)
{
	if (model->name() != "") {
		m_models[model->name()] = model;
	}
	else {
		m_models[std::to_string(model->id())] = model;
	}
}

/**
 * get a model from the world
 */
GameEngine::gaModel* gaWorld::getModel(const std::string& name)
{
	if (m_models.count(name) > 0) {
		return m_models[name];
	}

	return nullptr;
}

/**
 * remove a model from the world
 */
bool gaWorld::removeModel(const std::string& name)
{
	if (m_models.count(name) > 0) {
		m_models.erase(name);
		return true;
	}

	return false;
}

/**
 * return all models of a specific class
 */
void gaWorld::getModelsByClass(uint32_t myclass, std::list<GameEngine::gaModel*>& r)
{
	for (auto model : m_models) {
		if (model.second->modelClass() == myclass) {
			r.push_back(model.second);
		}
	}
}

/**
 * add the sprite maanager
 */
void gaWorld::spritesManager(dfSprites* sprites)
{
	m_sprites = sprites;
}

static int g_lastMessage = 0;
static gaMessage g_messages[2048];

/**
 * Allocate a new message
 */
static gaMessage* allocateMessage(void)
{
	// search for an available message
	int count = 2048;

	gaMessage* ptr = nullptr;
	do {
		if (--count < 0) {
			assert("not enough messages in gaWorld::getMessage");
		}
		ptr = &g_messages[g_lastMessage++];
		if (g_lastMessage == 2048) {
			g_lastMessage = 0;
		}
	} while (ptr->m_used);

	return ptr;
}

/**
 * send a message for immediate action
 */
gaMessage* gaWorld::sendMessage(const std::string& from, const std::string& to, int action, int value, void* extra)
{
	gaMessage* ptr = allocateMessage();
	ptr->m_used = true;

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = value;
	ptr->m_extra = extra;

	m_queue.push(ptr);

	return ptr;
}

void gaWorld::push(gaMessage* message)
{
	m_queue.push(message);
}

/**
 * send message for next frame
 */
gaMessage* gaWorld::sendMessageDelayed(const std::string& from, const std::string& to, int action, int value, void* extra)
{
	gaMessage* ptr = allocateMessage();
	ptr->m_used = true;

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = value;
	ptr->m_extra = extra;

	m_for_next_frame.push(ptr);

	return ptr;
}

void gaWorld::pushForNextFrame(gaMessage* message)
{
	m_for_next_frame.push(message);
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
void gaWorld::findAABBCollision(fwAABBox& box, std::list<gaEntity*>& entities, std::list<dfSuperSector*>& sectors)
{
	for (auto entry : m_entities) {
		// test all entity with the same name
		for (auto entity : entry.second) {
			if (entity->collideAABB(box)) {
				entities.push_back(entity);
			}
		}
	}

	for (auto sector : m_sectors) {
		if (sector->collideAABB(box)) {
			sectors.push_back(sector);
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
	std::list<dfSuperSector*> sectors;
	g_gaWorld.findAABBCollision(aabb, entities, sectors);

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

/**
 * dispatch messages
 */
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
		std::string k = message->m_server + message->m_client;
		if (loopDetector.count(k) > 0) {
			message->m_used = false;
			continue;
		}

		loopDetector[k] = true;

		if (message->m_action != GA_MSG_TIMER) {
			std::cerr << "gaWorld::process server=" << message->m_server << " action=" << message->m_action << " client=" << message->m_client << std::endl;;
		}

		// animation suspended ?
		if (!m_timer && message->m_action == GA_MSG_TIMER) {
			continue;
		}

		if (message->m_client == "_world") {
			// catch messages for the world
			switch (message->m_action) {
			case GA_MSG_DELETE_ENTITY:
				// delete all instances of the given entity
				if (m_entities.count(message->m_server) > 0) {
					for (auto entity : m_entities[message->m_server]) {
						delete entity;
						m_entities.erase(message->m_server);
						break;
					}
				}
			}
		}
		else if (m_entities.count(message->m_client) > 0) {
			// dispatch messages to client

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

	// update all sprites if needed
	if (m_sprites) {
		m_sprites->update();
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
