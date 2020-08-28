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

/**
 * send a message for immediate action
 */
gaMessage* gaWorld::sendMessage(const std::string& from, const std::string& to, int action, float value, void* extra)
{
	gaMessage* ptr = allocateMessage();
	ptr->m_used = true;

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_fvalue = value;
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

static void debugCollision(const gaCollisionPoint& collision, gaEntity* entity, const std::string& msg)
{
	std::string collider;

	if (collision.m_class == gaCollisionPoint::Source::ENTITY) {
		collider = static_cast<gaEntity*>(collision.m_source)->name();
	}
	else {
		collider = static_cast<dfSuperSector*>(collision.m_source)->name();
	}

	gaDebugLog(1, "gaWorld::wantToMove",
		entity->name() + " found collision " + msg
		+ "with " + collider
		+ " at " + std::to_string(collision.m_position.y)
	);
}

void gaWorld::wantToMove(gaEntity *entity, gaMessage *message)
{
	GameEngine::Transform* tranform = static_cast<GameEngine::Transform*>(message->m_extra);

	std::vector<gaEntity*> entities;
	std::vector<dfSuperSector*> sectors;
	std::vector<gaCollisionPoint> collisions;

	glm::vec3 old_position = entity->position();

	entity->pushTransformations();
	entity->transform(tranform);

	if (entity->name() == "elev3-5")
		gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " to " + std::to_string(tranform->m_position.x)
			+ " " + std::to_string(tranform->m_position.y)
			+ " " + std::to_string(tranform->m_position.z));

	if (message->m_value != gaMessage::Flag::FORCE_MOVE) {
		// do a warpThrough quick test
		glm::vec3 warp;
		fwAABBox aabb_ws(entity->position(), old_position);
		std::vector<glm::vec3> warps;

		for (auto sector : m_sectors) {
			if (sector->collideAABB(aabb_ws)) {
				// do a warpTrough full test
				if (entity->warpThrough(sector->collider(), old_position, warp)) {
					warps.push_back(warp);
				}
			}
		}

		if (warps.size() > 0) {
			float nearest = 9999999;
			glm::vec3 near_c = glm::vec3(0);

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
			gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " warp detected, fixed at " + std::to_string(near_c.y));
			return;
		}
	}

	// collide against the entities
	uint32_t size;

	for (auto entry : m_entities) {
		for (auto ent : entry.second) {
			if (ent != entity && entity->collideAABB(ent->worldAABB())) {
				size = collisions.size();
				if (entity->collide(ent, tranform->m_forward, tranform->m_downward, collisions)) {
					for (auto i = size; i < collisions.size(); i++) {
						collisions[i].m_source = ent;
						collisions[i].m_class = gaCollisionPoint::Source::ENTITY;
					}
				}
			}
		}
	}

	// collide against the sectors
	bool fall = false;
	bool fix_y = false;	// do we enter the ground and need Y to be fixed

	// elevators don't need to be checked against sectors
	if (message->m_value != gaMessage::Flag::FORCE_MOVE) {
		fall = true;
		for (auto sector : m_sectors) {
			size = collisions.size();
			if (sector->collideAABB(entity->worldAABB()) &&
				entity->collide(sector->collider(), tranform->m_forward, tranform->m_downward, collisions))
			{
				for (auto i = size; i < collisions.size(); i++) {
					collisions[i].m_source = sector;
					collisions[i].m_class = gaCollisionPoint::Source::SECTOR;
				}
			}
		}
	}

	// find the nearest collisions
	gaCollisionPoint* nearest_collision = nullptr;
	gaCollisionPoint* nearest_ground = nullptr;
	float distance = 99999999.0f;
	float ground = -9999999.0f;
	float d;
	float dBottom = 0;		// distance from worldAAB bottom to collision Y
	float dTop = 0;			// distance from worldAAB top to collision Y
	float lifted = 0;		// if collision on top for elevator, record the Y lifting
	float pushed_aside = 0;	// if collision on border of elevator, record the XZ direction

	for (auto& collision : collisions) {
		switch (collision.m_location) {
		case fwCollisionLocation::FRONT:
			d = entity->distanceTo(collision.m_position);
			if (d < distance) {
				nearest_collision = &collision;
				distance = d;
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
				if (d > ground) {
					nearest_ground = &collision;
					ground = d;
				}
			}
			if (entity->name() == "player") {
				gaDebugLog(1, "gaWorld::wantToMove", "BOTTOM detected at " + std::to_string(collision.m_position.y));
			}
			break;
		case fwCollisionLocation::COLLIDE: {
			// cylinder collision (player)
			// position of the intersection compared to the direction
			const fwAABBox& worldAABB = entity->worldAABB();

			dBottom = collision.m_position.y - worldAABB.m_p.y;
			dTop = worldAABB.m_p1.y - collision.m_position.y;

			fwCollisionLocation c;
			if (dBottom < 0.101) {				// BOTTOM
				debugCollision(collision, entity, "on bottom");

				if (dBottom > ground) {
					nearest_ground = &collision;
					ground = dBottom;
				}
			}
			else if (dTop < 0.101) {			// TOP
				debugCollision(collision, entity, "on top");

				d = entity->distanceTo(collision.m_position);
				if (d < distance) {
					nearest_collision = &collision;
					distance = d;
				}
				lifted = tranform->m_position.y - old_position.y;
			}
			else {							// FRONT/BACK/LEFT/RIGHT
				debugCollision(collision, entity, "on edge");

				d = entity->distanceTo(collision.m_position);
				if (d < distance) {
					nearest_collision = &collision;
					distance = d;
				}

				glm::vec2 p(tranform->m_position.x, tranform->m_position.z),
					p1(old_position.x, old_position.z);

				pushed_aside = glm::distance(p, p1);
			}
		}
		}
	}

	// take actions

	// manage ground collision and accept to jump up if over a step
	if (nearest_ground) {
		if (ground < 0.101) {
			if (ground != 0) {
				tranform->m_position.y = nearest_ground->m_position.y;
				fix_y = true;
			}
			gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " found ground at " + std::to_string(nearest_ground->m_position.y));
		}
	}
	else {
		// if there is no floor
		switch (message->m_value) {
		case gaMessage::Flag::WANT_TO_MOVE_LASER:
		case gaMessage::Flag::FORCE_MOVE:
			// ignore falling (for laser and elevators)
			break;

		case gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL:
			// if the entity wants to be informed of falling
			message->m_action = gaMessage::Action::WOULD_FALL;
			entity->popTransformations();			// restore previous position
			return;
		default:
			// if the entity wants to be informed of falling
			message->m_action = gaMessage::Action::FALL;
			gaDebugLog(1, "gaWorld::wantToMove", entity->name() + "falling");
			return;
		}
	}

	if (nearest_collision) {
		// if there is a collision

		if (nearest_collision->m_class == gaCollisionPoint::Source::ENTITY) {
			// inform a collided entity

			if (message->m_value == gaMessage::Flag::FORCE_MOVE) {
				// inform being lifted or pushed aside
				if (lifted > 0) {
					sendMessage(
						message->m_client,
						static_cast<gaEntity*>(nearest_collision->m_source)->name(),
						gaMessage::Action::LIFTED,
						lifted,
						nullptr
					);
				}
				else if (pushed_aside > 0) {
					sendMessage(
						message->m_client,
						static_cast<gaEntity*>(nearest_collision->m_source)->name(),
						gaMessage::Action::PUSHED_ASIDE,
						pushed_aside,
						nullptr
					);
				}
			}
			else {
				// just inform of a collision
				sendMessage(
					message->m_client,
					static_cast<gaEntity*>(nearest_collision->m_source)->name(),
					gaMessage::Action::COLLIDE,
					gaMessage::Flag::COLLIDE_ENTITY,
					nullptr
				);
			}
		}

		if (message->m_value != gaMessage::Flag::FORCE_MOVE) {
			gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " deny move " + std::to_string(tranform->m_position.x)
					+ " " + std::to_string(tranform->m_position.y)
					+ " " + std::to_string(tranform->m_position.z));

			// and we do not force the move
			// refuse the move and inform both element from the collision
			message->m_action = gaMessage::Action::COLLIDE;
			if (nearest_collision->m_class == gaCollisionPoint::Source::ENTITY) {
				message->m_server = static_cast<gaEntity*>(nearest_collision->m_source)->name();
				message->m_value = gaMessage::Flag::COLLIDE_ENTITY;
			}
			else {
				message->m_server = static_cast<dfSuperSector*>(nearest_collision->m_source)->name();
				message->m_value = gaMessage::Flag::COLLIDE_WALL;
			}
			entity->popTransformations();				// restore previous position
			return;
		}
	}

	// accept the move
	if (message->m_action == gaMessage::WANT_TO_MOVE) {
		message->m_action = gaMessage::MOVE;
		if (!fix_y) {
			message->m_extra = nullptr;					// object was correctly moved
		}
		else {
			gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " fixed ground " + std::to_string(tranform->m_position.x)
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
	gaMessage* message;
	std::string k;

	std::map<std::string, bool> loopDetector;

	while (m_queue.size() > 0) {
		message = m_queue.front();
		m_queue.pop();

		// manage loops inside one run
		k = message->m_server + message->m_client + std::to_string(message->m_action);
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
