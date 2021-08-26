#include "World.h"

#include <iostream>
#include <map>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "../framework/fwScene.h"
#include "../framework/fwMesh.h"
#include "../framework/fwTransforms.h"

#include "gaEntity.h"
#include "Physics.h"

#include "../darkforces/dfLevel.h"
#include "../darkforces/dfSuperSector.h"
#include "../darkforces/dfSprites.h"

#include "../flightRecorder/Blackbox.h"

using namespace GameEngine;

GameEngine::World g_gaWorld;

/**
 * Create and initialize the world
 */
World::World():
	m_scene(nullptr),
	m_physic(this)
{
}

/**
 * Let the game engine process events
 */
void GameEngine::World::run(void)
{
	m_run = true;
}

/**
 * Suspend the game engine, no event will be processed
 */
void GameEngine::World::suspend(void)
{
	m_run = false;
}

/**
 *  register the scene
 */
void World::scene(fwScene* scene)
{
	m_scene = scene;
	set("scene", scene);
}

/**
 * Add a new gaEntity
 */
void World::addClient(gaEntity* client)
{
	m_entities[client->name()].push_back(client);
	client->OnWorldInsert();
	sendImmediateMessage("_world", client->name(), gaMessage::WORLD_INSERT, 0, nullptr);
}

/**
 * remove a gaEntity
 */
void World::removeClient(gaEntity* client)
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
void World::add2scene(gaEntity* client)
{
	client->add2scene(m_scene);
}

void World::add2scene(fwMesh* mesh)
{
	m_scene->addChild(mesh);
}

/**
 * Remove a mesh from the scene
 */
void World::remove2scene(fwMesh* mesh)
{
	m_scene->removeChild(mesh);
}

/**
 * add a game sector
 */
void World::addSector(dfSuperSector* client)
{
	m_sectors.push_back(client);
}

/**
 * find the super sector the position is inside
 *  used the last known super sector as base
 */
dfSuperSector* World::findSector(dfSuperSector* cached, const glm::vec3& position)
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
void World::set(const std::string& name, void* object)
{
	m_registry[name] = object;
}

/**
 * retrieve a generic object
 */
void* World::get(const std::string& name)
{
	if (m_registry.count(name) > 0) {
		return m_registry[name];
	}

	return nullptr;
}

/**
 * add a new model to the world
 */
void World::addModel(GameEngine::Model* model)
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
Model* World::getModel(const std::string& name)
{
	if (m_models.count(name) > 0) {
		return m_models[name];
	}

	return nullptr;
}

/**
 * remove a model from the world
 */
bool World::removeModel(const std::string& name)
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
void World::getModelsByClass(uint32_t myclass, std::list<GameEngine::Model*>& r)
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
void World::spritesManager(dfSprites* sprites)
{
	m_sprites = sprites;
	sprites->OnWorldInsert();
}

#define MAXIMUM_MESSAGES 2048
static int g_lastMessage = 0;
static gaMessage g_messages[MAXIMUM_MESSAGES];

/**
 * Allocate a new message
 */
static gaMessage* allocateMessage(void)
{
	// search for an available message
	int count = 2048;

	gaMessage* ptr = nullptr;
	do {
		--count;
		assert(count > 0, "not enough messages in gaWorld::getMessage");

		ptr = &g_messages[g_lastMessage++];
		if (g_lastMessage == MAXIMUM_MESSAGES) {
			g_lastMessage = 0;
		}
	} while (ptr->m_used);

	return ptr;
}

/**
 * send a message for immediate action
 */
gaMessage* World::sendMessage(const std::string& from, const std::string& to, int action, int value, void* extra)
{
	gaMessage* ptr = allocateMessage();
	ptr->m_used = true;

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = value;
	ptr->m_extra = extra;

	m_queue.push_back(ptr);

	return ptr;
}

/**
 * send a message for immediate action
 */
gaMessage* World::sendMessage(const std::string& from, const std::string& to, int action, float value, void* extra)
{
	gaMessage* ptr = allocateMessage();
	ptr->m_used = true;

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_fvalue = value;
	ptr->m_extra = extra;

	m_queue.push_back(ptr);

	return ptr;
}

/**
 * send a message for immediate action
 */
gaMessage* World::sendMessage(const std::string& from, const std::string& to, int action, const glm::vec3& value, void* extra)
{
	gaMessage* ptr = allocateMessage();
	ptr->m_used = true;

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_v3value = value;
	ptr->m_extra = extra;

	m_queue.push_back(ptr);

	return ptr;
}

/**
 * Replay the flight recorder
 */
gaMessage* GameEngine::World::sendMessage(flightRecorder::Message* record)
{
	gaMessage* ptr = allocateMessage();
	ptr->m_used = true;

	ptr->m_client = record->client;
	ptr->m_server = record->server;
	ptr->m_action = record->action;
	ptr->m_delta = record->delta;
	ptr->m_fvalue = record->fvalue;
	ptr->m_v3value = record->v3value;
	ptr->m_value = record->value;

	m_queue.push_back(ptr);

	return ptr;
}

void World::push(gaMessage* message)
{
	m_queue.push_back(message);
}

/**
 * send message for next frame
 */
gaMessage* World::sendMessageDelayed(const std::string& from, const std::string& to, int action, int value, void* extra)
{
	gaMessage* ptr = allocateMessage();
	ptr->m_used = true;

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = value;
	ptr->m_extra = extra;

	m_for_next_frame.push_back(ptr);

	return ptr;
}

/**
 * send message for immediate dispatch
 */
gaMessage* World::sendImmediateMessage(const std::string& from, const std::string& to, int action, int value, void* extra)
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

void World::pushForNextFrame(gaMessage* message)
{
	m_for_next_frame.push_back(message);
}


/**
 * Search the entities map
 */
gaEntity* World::getEntity(const std::string& name)
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
void World::findAABBCollision(const fwAABBox& box, 
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
bool World::checkCollision(gaEntity* source, fwCylinder& bounding, glm::vec3& direction, std::list<gaCollisionPoint>& collisions)
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
 * dispatch messages
 */
void World::process(time_t delta, bool force)
{
	// is the engine actually running ?
	if (!m_run && !force) {
		return;
	}

	m_frame++;

#ifdef _DEBUG
	// record start at start of frame
	g_Blackbox.recordState();
#endif

	/*
	if (m_queue.size() > 0) {
		std::cerr << ">>>>>>>>>> gaWorld::process" << std::endl;
	}
	*/

	gaMessage* message;
	std::string k;

	std::map<std::string, bool> loopDetector;

	// first deal with falling objects
	m_physic.update(delta);

	// now deal with messages
	while (m_queue.size() > 0) {
		message = m_queue.front();

#ifdef _DEBUG
		// record messages on the fly
		g_Blackbox.recordMessage(message);
#endif

		m_queue.pop_front();

		// manage loops inside one run
		if (message->m_action >= 512) {
			// only test for loop for DF messages
			k = message->m_server + message->m_client + std::to_string(message->m_action);
			if (loopDetector.count(k) > 0) {
				message->m_used = false;
				continue;
			}

			loopDetector[k] = true;
		}

		//if (message->m_action != gaMessage::TIMER && message->m_action != gaMessage::WANT_TO_MOVE) {
		//	std::cerr << "gaWorld::process server=" << message->m_server << " action=" << message->m_action << " client=" << message->m_client << std::endl;;
		//}

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
			case gaMessage::SAVE_WORLD:
				g_Blackbox.saveStates();
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

				if (message->m_action == gaMessage::Action::WANT_TO_MOVE) {
					m_physic.moveEntity(entity, message);
				}
				else {
					message->m_frame = m_frame;
					entity->dispatchMessage(message);
				}
			}
		}
		message->m_used = false;
	}

	update();	// update extra attributes of the world

	// swap the current queue and the queue for next frame
	m_queue.swap(m_for_next_frame);
}

void World::suspendTimer(void)
{
	m_timer = false;
}

/**
 * Render the list of entities on the debug imGUI
 */
void GameEngine::World::debugGUI(void)
{
	// list entities to pick from
	static bool eclose = false;

	if (!eclose && ImGui::Begin("Explorer", &eclose)) {
		if (ImGui::CollapsingHeader("gaEntities")) {
			for (auto entry : m_entities) {
				for (auto ent : entry.second) {
					const std::string& name = ent->name();
					bool old = m_watch[name];
					ImGui::Checkbox(name.c_str(), &m_watch[name]);
					if (old != m_watch[name]) {
						if (m_watch[name]) {
							ent->worldAABB().color(glm::vec3(1.0f, 0.0f, 0.0f));
						}
						else {
							ent->worldAABB().color(glm::vec3(1.0f, 1.0f, 1.0f));
						}
					}
				}
			}
		}
		ImGui::End();

	}
	// display entities monitored
	ImGui::Begin("Inspector");
	for (auto &watch : m_watch) {
		if (watch.second) {
			auto &entities = m_entities[watch.first];
			for (auto entity : entities) {
				entity->debugGUI(&m_watch[watch.first]);
			}
		}
	}
	ImGui::End();

	// display messages
	static bool mclose = false;
	if (!mclose && ImGui::Begin("Messages", &mclose)) {
		if (ImGui::BeginTable("Messages", 5, ImGuiTableFlags_Resizable)) {
			ImGui::TableSetupColumn("From");
				ImGui::TableSetupColumn("To");
				ImGui::TableSetupColumn("Action");
				ImGui::TableSetupColumn("iValue");
				ImGui::TableSetupColumn("fValue");
				ImGui::TableHeadersRow();

				for (auto message : m_queue) {
					message->debugGUI();
				}
			ImGui::EndTable();
		}
		ImGui::End();
	}
}

/**
 * render the imGUI debug messages
 */
void GameEngine::World::debugGUImessages(std::list<gaMessage>& l)
{
	// display messages
	ImGui::Begin("Messages");
	if (ImGui::BeginTable("inframe Messages", 5, ImGuiTableFlags_Resizable)) {
		ImGui::TableSetupColumn("From");
		ImGui::TableSetupColumn("To");
		ImGui::TableSetupColumn("Action");
		ImGui::TableSetupColumn("iValue");
		ImGui::TableSetupColumn("fValue");
		ImGui::TableHeadersRow();

		for (auto& message : l) {
			message.debugGUI();
		}
		ImGui::EndTable();
	}
	ImGui::End();
}

/**
 * Clear the message queue
 */
void GameEngine::World::clearQueue(void)
{
	m_queue.clear();

	for (auto& message : g_messages) {
		message.m_used = false;
	}
	g_lastMessage = 0;
}

/**
 * force an update of the world
 */
void GameEngine::World::update(void)
{
	// update all sprites if needed
	if (m_sprites) {
		m_sprites->update();
	}
}

/**
 * find the all entities intersecting with the segment
 */
bool GameEngine::World::intersectWithEntity(
	uint32_t componentID, 
	const Framework::Segment& segment, 
	std::vector<gaEntity*>& collisions)
{
	fwAABBox segment_aabb(segment.m_start, segment.m_end);

	std::vector<gaEntity*> entities;
	
	getEntitiesWithComponents(componentID, entities);

	// test again entities
	for (auto ent: entities) {
		if (ent->worldAABB().intersect(segment_aabb)) {
			collisions.push_back(ent);
		}
	}

	return false;
}

/**
 * return all entities with a special components
 */
void GameEngine::World::getEntitiesWithComponents(uint32_t componentID, std::vector<gaEntity*>& entities)
{
	// test again entities
	for (auto& entry : m_entities) {
		for (auto ent : entry.second) {
			if (ent->findComponent(componentID) != nullptr) {
				entities.push_back(ent);
			}
		}
	}
}

World::~World()
{
}
