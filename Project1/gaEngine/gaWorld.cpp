#include "gaWorld.h"

#include <iostream>
#include <map>

#include "../framework/fwScene.h"
#include "../framework/fwMesh.h"
#include "../framework/fwTransforms.h"

#include "gaEntity.h"

#include "../darkforces/dfLevel.h"
#include "../darkforces/dfSuperSector.h"
#include "../darkforces/dfSprites.h"

gaWorld g_gaWorld;

/**
 * Create and initialize the world
 */
gaWorld::gaWorld():
	m_scene(nullptr)
{
}

/**
 *  register the scene
 */
void gaWorld::scene(fwScene* scene)
{
	m_scene = scene;
	set("scene", scene);
}

/**
 * Add a new gaEntity
 */
void gaWorld::addClient(gaEntity* client)
{
	m_entities[client->name()].push_back(client);
	client->OnWorldInsert();
	sendImmediateMessage("_world", client->name(), gaMessage::WORLD_INSERT, 0, nullptr);
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

	client->OnWorldRemove();
	sendImmediateMessage("_world", client->name(), gaMessage::WORLD_REMOVE, 0, nullptr);

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

void gaWorld::add2scene(fwMesh* mesh)
{
	m_scene->addChild(mesh);
}

/**
 * Remove a mesh from the scene
 */
void gaWorld::remove2scene(fwMesh* mesh)
{
	m_scene->removeChild(mesh);
}

/**
 * add a game sector
 */
void gaWorld::addSector(dfSuperSector* client)
{
	m_sectors.push_back(client);
}

/**
 * find the super sector the position is inside
 *  used the last known super sector as base
 */
dfSuperSector* gaWorld::findSector(dfSuperSector* cached, const glm::vec3& position)
{
	void* sector;

	// position is in opengl space
	// TODO should move the opengl <-> level space conversion on a central place
	glm::vec3 level_space;
	dfLevel::gl2level(position, level_space);

	// quick check on the last super sector
	if (cached) {
		if (cached->isPointInside(position)) {
			return cached;
		}
	}

	// moved out
	for (auto ssector : m_sectors) {
		sector = ssector->findSector(position);

		if (sector) {
			return ssector;
		}
	}

	// force on the last position
	return nullptr;
}

/**
 * add a new generic object
 */
void gaWorld::set(const std::string& name, void* object)
{
	m_registry[name] = object;
}

/**
 * retrieve a generic object
 */
void* gaWorld::get(const std::string& name)
{
	if (m_registry.count(name) > 0) {
		return m_registry[name];
	}

	return nullptr;
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
 * add the sprite manager
 */
void gaWorld::spritesManager(dfSprites* sprites)
{
	m_sprites = sprites;
	sprites->OnWorldInsert();
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

/**
 * send message for immediate dispatch
 */
gaMessage* gaWorld::sendImmediateMessage(const std::string& from, const std::string& to, int action, int value, void* extra)
{
	gaMessage* ptr = allocateMessage();
	ptr->m_used = true;

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = value;
	ptr->m_extra = extra;

	if (m_entities.count(to) > 0) {
		// dispatch messages to client
		if (m_entities.count(from) > 0) {
			ptr->m_pServer = m_entities[from].front();
		}
		for (auto entity : m_entities[to]) {
			entity->dispatchMessage(ptr);
		}
	}
	ptr->m_used = false;

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
void gaWorld::findAABBCollision(const fwAABBox& box, 
	std::list<gaEntity*>& entities, 
	std::list<dfSuperSector*>& sectors, 
	gaEntity* source)
{
	for (auto entry : m_entities) {
		// test all entity with the same name
		for (auto entity : entry.second) {

			// skip the requester
			if (entity == source || entity->name() == "player") {
				continue;
			}

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
 * extended collision test after a successful AABB collision
 */
bool gaWorld::checkCollision(gaEntity* source, fwCylinder& bounding, glm::vec3& direction, std::list<gaCollisionPoint>& collisions)
{
	glm::vec3 intersection;

	// get all the entities which AABB checkCollision with the player
	fwAABBox aabb(bounding);
	aabb += direction;
	std::list<gaEntity*> entities;
	std::list<dfSuperSector*> sectors;
	g_gaWorld.findAABBCollision(aabb, entities, sectors, source);

	for (auto entity : entities) {
		if (entity == source) {
			// ignore self
			continue;
		}

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
 * An entity wants to move
 */
void gaWorld::wantToMove(gaEntity *entity, gaMessage *message)
{
	gaEntity* nearest_entity = nullptr;
	dfSuperSector* nearest_sector = nullptr;
	GameEngine::Transform* tranform = static_cast<GameEngine::Transform*>(message->m_extra);

	float distance = 99999999.0f;
	float distance_sector = 99999999.0f;
	float d;

	std::list<gaEntity*> entities;
	std::list<dfSuperSector*> sectors;
	std::list<gaCollisionPoint> collisions;

	glm::vec3 old_position = entity->position();

	entity->pushTransformations();
	entity->transform(tranform);

	if (entity->name() == "player")
		gaDebugLog(1, "gaWorld::wantToMove", "to " + std::to_string(tranform->m_position.x)
			+ " " + std::to_string(tranform->m_position.y)
			+ " " + std::to_string(tranform->m_position.z));

	// do a warpThrough quick test
	fwAABBox aabb_ws(entity->position(), old_position);
	glm::vec3 collision;
	std::vector<glm::vec3> warps;

	for (auto sector : m_sectors) {
		if (sector->collideAABB(aabb_ws)) {
			// do a warpTrough full test
			if (entity->warpThrough(sector->collider(), old_position, collision)) {
				warps.push_back(collision);
			}
		}
	}

	if (warps.size() > 0) {
		float nearest = 9999999;
		glm::vec3 near_c=glm::vec3(0);

		// find the nearest point
		for (auto& collision : warps) {
			if (glm::distance2(old_position, collision) < nearest) {
				nearest = glm::distance2(old_position, collision);
				near_c = collision;
			}
		}
		// and force the object there
		message->m_action = gaMessage::MOVE;
		tranform->m_position = near_c;
		if (entity->name() == "player") {
			gaDebugLog(1, "gaWorld::wantToMove", "warp detected, fixed at " + std::to_string(near_c.y));
		}
		return;
	}

	// do an AABB collision against AABB with entities
	findAABBCollision(entity->worldAABB(), entities, sectors, entity);

	//if (entity->name() == "player" && tranform->m_forward != glm::vec3(0)) {
	//	printf("gaWorld::wantToMove\n");
	//}
	// and then collide the objects
	for (auto ent : entities) {
		if (entity->collide(ent, tranform->m_forward, tranform->m_downward, collisions)) {
			// only test entities that can physically checkCollision
			d = ent->distanceTo(entity);
			if (d < distance) {
				nearest_entity = ent;
				distance = d;
			}
		}
	}

	// do an segment collision against the sectors triangles
	bool fall = true;
	bool fix_y = false;	// do we enter the ground and need Y to be fixed

	for (auto sector : sectors) {
		if (entity->collide(sector->collider(), tranform->m_forward, tranform->m_downward, collisions)) {
			// no warping, so test deeper
			for (auto& collision : collisions) {
				switch (collision.m_location) {
				case fwCollisionLocation::FRONT:
					d = entity->distanceTo(collision.m_position);
					if (d < distance_sector) {
						nearest_sector = sector;
						distance_sector = d;
					}
					fall = false;
					if (entity->name() == "player") {
						gaDebugLog(1, "gaWorld::wantToMove", "FRONT detected at " + std::to_string(collision.m_position.y));
					}
					break;
				case fwCollisionLocation::BOTTOM:
					fall = false;
					if (message->m_value != gaMessage::Flag::WANT_TO_MOVE_LASER) {
						float d = abs(tranform->m_position.y - collision.m_position.y);
						if (d < 0.5) {
							tranform->m_position.y = collision.m_position.y;		// move up the stair
						}
					}
					if (entity->name() == "player") {
						gaDebugLog(1, "gaWorld::wantToMove", "BOTTOM detected at " + std::to_string(collision.m_position.y));
					}
					break;
				case fwCollisionLocation::COLLIDE: {
					// cylinder collision (player)
					// position of the intersection compared to the direction
					const fwAABBox& bb = entity->modelAABB();
					glm::vec3 p = entity->position();
					// p.y += bb.center().y;
					float d = collision.m_position.y - p.y;

					/*
					glm::vec3 AC = glm::normalize(entity->position() - collision.m_position);
					float f = 0;
					if (tranform->m_forward != glm::vec3(0)) {
						f = glm::dot(glm::normalize(tranform->m_forward), AC);
					}
					float d = glm::dot(glm::normalize(tranform->m_downward), AC);
					*/
					fwCollisionLocation c;
					if (d < 0.101) {
						c = fwCollisionLocation::BOTTOM;
						if (d != 0) {
							tranform->m_position.y = collision.m_position.y;
							fix_y = true;
						}
						fall = false;
						if (entity->name() == "player") {
							gaDebugLog(1, "gaWorld::wantToMove", "found ground at " + std::to_string(collision.m_position.y));
						}
					}
					else if (d > bb.m_p1.y) {
						c = fwCollisionLocation::TOP;
						tranform->m_position.y = collision.m_position.y - bb.m_p1.y;
						if (entity->name() == "player") {
							gaDebugLog(1, "gaWorld::wantToMove", "found ceiling at " + std::to_string(collision.m_position.y));
						}
					}
					else {
						if (entity->name() == "player") {
							gaDebugLog(1, "gaWorld::wantToMove", "collide at " + std::to_string(collision.triangle()));
						}
						c = fwCollisionLocation::COLLIDE;
						d = entity->distanceTo(collision.m_position);
						if (d < distance_sector) {
							nearest_sector = sector;
							distance_sector = d;
						}
					}
				}
				}
			}
		}
	}

	if (fall && message->m_value != gaMessage::Flag::WANT_TO_MOVE_LASER) {
		// if there is no floor
		switch (message->m_value) {
		case gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL:
			// if the entity wants to be informed of falling
			message->m_action = gaMessage::Action::WOULD_FALL;
			entity->popTransformations();			// restore previous position
			break;
		default:
			// if the entity wants to be informed of falling
			message->m_action = gaMessage::Action::FALL;
			if (entity->name() == "player") {
				gaDebugLog(1, "gaWorld::wantToMove", "falling");
			}
			break;
		}
	}
	else if (distance < 99999999.0f || distance_sector < 99999999.0f) {
		message->m_action = gaMessage::gaMessage::COLLIDE;
		if (distance < 99999999.0f) {
			message->m_server = nearest_entity->name();
			message->m_value = gaMessage::Flag::COLLIDE_ENTITY;
		}
		else {
			message->m_value = gaMessage::Flag::COLLIDE_WALL;
		}
		entity->popTransformations();				// restore previous position
	}
	else {
		// accept the move
		message->m_action = gaMessage::MOVE;

		if (!fix_y) {
			message->m_extra = nullptr;					// object was correctly moved
		}
		else {
			if (entity->name() == "player")
			gaDebugLog(1, "gaWorld::wantToMove", "fixed " + std::to_string(tranform->m_position.x)
				+ " " + std::to_string(tranform->m_position.y)
				+ " " + std::to_string(tranform->m_position.z));

		}
	}
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
		std::string k = message->m_server + message->m_client + std::to_string(message->m_action);
		if (loopDetector.count(k) > 0) {
			message->m_used = false;
			continue;
		}

		loopDetector[k] = true;

		if (message->m_action != gaMessage::TIMER && message->m_action != gaMessage::WANT_TO_MOVE) {
			std::cerr << "gaWorld::process server=" << message->m_server << " action=" << message->m_action << " client=" << message->m_client << std::endl;;
		}

		// animation suspended ?
		if (!m_timer && message->m_action == gaMessage::TIMER) {
			continue;
		}

		if (message->m_client == "_world") {
			// catch messages for the world
			switch (message->m_action) {
			case gaMessage::DELETE_ENTITY:
				// delete all instances of the given entity
				if (m_entities.count(message->m_server) > 0) {
					for (auto entity : m_entities[message->m_server]) {
						sendImmediateMessage("_world", entity->name(), gaMessage::WORLD_REMOVE, 0, nullptr);
						entity->OnWorldRemove();
						delete entity;
						m_entities.erase(message->m_server);
						break;
					}
				}
				break;
			}
		}
		else if (m_entities.count(message->m_client) > 0) {
			// dispatch messages to client

			message->m_delta = delta;
			if (m_entities.count(message->m_server) > 0) {
				message->m_pServer = m_entities[message->m_server].front();
			}
			for (auto entity : m_entities[message->m_client]) {

				if (message->m_action == gaMessage::WANT_TO_MOVE) {
					wantToMove(entity, message);
				}

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
