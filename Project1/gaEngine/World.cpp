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
#include "gaComponent/gaAIPerception.h"
#include "gaComponent/gaCActor.h"

#include "../darkforces/dfLevel.h"
#include "../darkforces/dfSuperSector.h"
#include "../darkforces/dfSprites.h"

#include "../flightRecorder/Blackbox.h"

using namespace GameEngine;

static std::map<uint32_t, const char*> g_entityClassName;

GameEngine::World g_gaWorld;

/**
 * Create and initialize the world
 */
World::World():
	m_scene(nullptr)
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

	uint32_t mclass = client->mclass();
	m_entitiesByClass[mclass].push_back(client);

	// register the class name
	if (g_entityClassName.count(mclass) == 0) {
		g_entityClassName[mclass] = client->className();
	}

	sendImmediateMessage("_world", client->name(), gaMessage::WORLD_INSERT, 0, nullptr);
}

/**
 * remove a gaEntity
 */
void World::removeClient(gaEntity* client)
{
	sendImmediateMessage("_world", client->name(), gaMessage::WORLD_REMOVE, 0, nullptr);

	// remove from the list
	m_entities[client->name()].remove(client);

	// if the list is empty remove the name
	if (m_entities[client->name()].size() == 0) {
		m_entities.erase(client->name());
	}

	m_entitiesByClass[client->mclass()].remove(client);

	registerTimerEvents(client, false);
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
		if (cached->inAABBox(position)) {
			return cached;
		}
	}

	// moved out
	for (auto ssector : m_sectors) {
		sector = ssector->findDFSector(position);

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
static uint32_t g_MsgID = 0;
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
		assert(count > 0);

		ptr = &g_messages[g_lastMessage++];
		if (g_lastMessage == MAXIMUM_MESSAGES) {
			g_lastMessage = 0;
		}
	} while (ptr->m_used);

	ptr->m_used = true;
	ptr->m_id = g_MsgID++;
	ptr->m_canceled = false;

	return ptr;
}

/**
 * send a message for immediate action
 */
gaMessage* World::sendMessage(const std::string& from, const std::string& to, int action, int value, void* extra)
{
	gaMessage* ptr = allocateMessage();

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = value;
	ptr->m_fvalue = 0;
	ptr->m_extra = extra;

	m_queue.push_back(ptr);

	return ptr;
}

gaMessage* World::sendMessage(const std::string& from, const std::string& to, int action, int value, float fvalue, void* extra)
{
	gaMessage* ptr = allocateMessage();

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = value;
	ptr->m_fvalue = fvalue;
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

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = 0;
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

	ptr->m_server = from;
	ptr->m_client = to;
	ptr->m_action = action;
	ptr->m_value = 0;
	ptr->m_fvalue = 0;
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
 * delete a previously submitted message
 */
bool World::deleteMessage(uint32_t id)
{
	// find and delete direct message
	for(auto &message: m_queue) {
		if (message->m_id == id) {
			message->m_canceled = true;
			return true;
		}
	}

	for (auto& message : m_for_next_frame) {
		if (message->m_id == id) {
			message->m_canceled = true;
			return true;
		}
	}

	//Action::MOVE,COLLIDE and WOULD_FALL are triggered by an WANT_TO_MOVE, so check indirectly
	bool moveAction = false;
	for (auto& message : m_queue) {
		if ((
			message->m_action == gaMessage::Action::MOVE || 
			message->m_action == gaMessage::Action::WOULD_FALL ||
			message->m_action == gaMessage::Action::COLLIDE)
			&& 
			message->m_value == id) 
		{
			message->m_canceled = true;
			moveAction=true;
		}
	}

	return moveAction;
}

/**
 * delete all messages for that entity
 */
void World::deleteMessages(gaEntity* entity)
{
	for (auto& message : m_queue) {
		if (message->m_client == entity->name()) {
			message->m_canceled = true;
		}
	}
	for (auto& message : m_for_next_frame) {
		if (message->m_client == entity->name()) {
			message->m_canceled = true;
		}
	}
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
 * Search the entities map
 */
bool World::getEntities(uint32_t type, std::vector<gaEntity*>& entities)
{
	// search new entities
	for (auto& entry : m_entities) {
		// test all entity with the same name
		for (auto entity : entry.second) {
			if (entity->is_class(type)) {
				entities.push_back(entity);
			}
		}
	}

	return entities.size() != 0;
}

/**
 * parse entities to check for collision with the given one
 */
void World::findAABBCollision(const fwAABBox& box, 
	std::list<gaEntity*>& entities, 
	std::list<dfSuperSector*>& sectors, 
	gaEntity* source)
{
	for (auto& entry : m_entities) {
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
	time_t t = GetTickCount64();

	// inject alarm event if needed
	std::vector<Alarm*> removeAlarm;

	for (auto& alarm : m_alarms) {
		alarm.m_delay -= delta;
		if (alarm.m_delay < 0) {
			sendMessage(alarm.m_entity->name(), alarm.m_entity->name(), alarm.m_message, 0, nullptr);
			removeAlarm.push_back(&alarm);
		}
	}
	for (auto alarm : removeAlarm) {
		m_alarms.remove(*alarm);
	}

	// inject timer messages if the entity request so
	// only way to ensure there is not multiple timer message coming from different components of the entity
	for (auto entity : m_timers) {
		sendMessage(entity->name(), entity->name(), gaMessage::TIMER, 0, nullptr);
	}

	// inject view perception events
	checkPerceptions();

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
	g_gaPhysics.update(delta);

	// now deal with messages
	while (m_queue.size() > 0) {
		message = m_queue.front();

#ifdef _DEBUG
		// record messages on the fly
		g_Blackbox.recordMessage(message);
#endif

		m_queue.pop_front();

		// someone canceled the message
		if (message->m_canceled) {
			message->m_used = false;
			continue;
		}

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
					std::vector<gaEntity*>p;

					for (auto entity : m_entities[message->m_server]) {
						p.push_back(entity);
					}

					for (auto entity: p) {
						removeClient(entity);
						delete entity;
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
			message->m_time = t;

			if (m_entities.count(message->m_server) > 0) {
				message->m_pServer = m_entities[message->m_server].front();
			}
			for (auto entity : m_entities[message->m_client]) {

				if (message->m_action == gaMessage::Action::WANT_TO_MOVE) {
					g_gaPhysics.moveEntity(entity, message);
				}
				else if (entity->processMessages()) {
					// only pass messages to entity that request so
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
 * test the line of the sight between 2 entities
 */
bool GameEngine::World::lineOfSight(gaEntity* source, gaEntity* target)
{
	// can we reach the player ?
	glm::vec3 direction = glm::normalize(source->position() - target->position());

	// test line of sight from await of the entity to away from the player (to not catch the entity nor the player)
	glm::vec3 start = target->position() + direction * (target->radius() * 1.5f);
	start.y += target->height() / 2.0f;
	glm::vec3 end = source->position() - direction * (target->radius() * 1.5f);
	end.y += source->height() / 2.0f;
	Framework::Segment segment(start, end);

	bool canSee = true;
	std::vector<gaEntity*> collisions;
	if (g_gaWorld.intersectWithEntity(segment, collisions)) {
		// check if there is a collision with something different than player and shooter
		for (auto entity : collisions) {
			if (entity != source && entity != target) {
				canSee = false;
				break;
			}
		}
	}

	return canSee;
}

/**
 * find the all entities intersecting with the segment
 */
bool GameEngine::World::intersectWithEntity(
	uint32_t componentID, 
	GameEngine::Collider& segment, 
	std::vector<gaEntity*>& collisions)
{
	std::vector<gaEntity*> entities;
	glm::vec3 p;
	getEntitiesWithComponents(componentID, entities);

	// test again entities
	for (auto ent: entities) {
		fwAABBox::Intersection r = ent->intersect(segment, p);
		if (r != fwAABBox::Intersection::NONE) {
			// if segment intersect or is included in the entity
			collisions.push_back(ent);
		}
	}

	return false;
}

bool GameEngine::World::intersectWithEntity(Framework::Segment& segment,
	std::vector<gaEntity*>& entCollisions)
{
	fwAABBox aabb(segment);
	glm::mat4 worldMatrix(1);
	glm::mat4 inverse = glm::inverse(worldMatrix);
	GameEngine::Collider collider(&segment, &worldMatrix, &inverse, &aabb);

	glm::vec3 forward(0), down(0);
	std::vector<gaCollisionPoint> collisions;

	gaEntity* collidedEntity = nullptr;

	// test again entities
	for (auto& entry : m_entities) {
		for (auto ent : entry.second) {
			// ignore ghosts and itself
			if (!ent->physical()) {
				continue;
			}

			// quick test
			if (ent->collideAABB(aabb)) {
				// extended test
				collisions.clear();
				if (ent->collide(collider, forward, down, collisions)) {
					entCollisions.push_back(ent);
				}
			}
		}
	}

	return entCollisions.size() > 0;
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

/**
 * (de)register an entity to receive timer events
 */
void GameEngine::World::registerTimerEvents(gaEntity* entity, bool b) 
{
	if (b) {
		// register
		if (std::find(m_timers.begin(), m_timers.end(), entity) != m_timers.end()) {
			// already registered
			return;
		}

		m_timers.push_back(entity);
	}
	else {
		m_timers.remove(entity);
	}
}

/**
 * register an entity to receive alarm event
 */
uint32_t GameEngine::World::registerAlarmEvent(Alarm& alarm)
{
	static uint32_t g_id = 0;

	alarm.m_id = g_id++;
	m_alarms.push_back(alarm);

	return alarm.m_id;
}

/**
 * cancel a programmed alarm
 */
bool GameEngine::World::cancelAlarmEvent(uint32_t id)
{
	Alarm* a = nullptr;
	for (auto& alarm : m_alarms) {
		if (alarm.m_id == id) {
			a = &alarm;
			break;
		}
	}

	if (a != nullptr) {
		m_alarms.remove(*a);
		return true;
	}

	return false;
}

/**
 *  (de)register entities for visual perceptions
 */
void GameEngine::World::registerViewEvents(gaEntity* entity)
{
	uint32_t id = entity->entityID();
	if (m_views.count(id) == 0) {
		m_views[id] = entity;
	}
}

void GameEngine::World::deRegisterViewEvents(gaEntity* entity)
{
	uint32_t id = entity->entityID();
	if (m_views.count(id) != 0) {
		m_views.erase(id);
	}
}

void GameEngine::World::checkPerceptions(void)
{
	gaEntity* player = getEntity("player");
	gaEntity* entity;
	Component::AIPerception* perception;
	Component::Actor* actor;

	for(auto& pair: m_views) {
		entity = pair.second;
		perception = dynamic_cast<Component::AIPerception*>(entity->findComponent(gaComponent::AIPerception));

		// the player is in the entity distance perception
		if (player->distanceTo(entity) < perception->distance()) {
			actor = dynamic_cast<Component::Actor*>(entity->findComponent(gaComponent::Actor));

			glm::vec3 d = player->position() - entity->position();
			glm::vec3 v = actor->direction();

			//printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", player->position().x, player->position().z, entity->position().x, entity->position().z, actor->direction().x, actor->direction().z);
			// the player is in front of the entity
			if (glm::dot(d, v) > 0) {

				//TODO: the player is in the entity cone of vision
				
				// the player is in the line of sight of the entity
				if (lineOfSight(player, entity)) {
					sendMessage(player->name(), entity->name(), gaMessage::Action::VIEW, player->position(), nullptr);
				}
			}
		}
	}
}

/**
 * Render the list of entities on the debug imGUI
 */
void GameEngine::World::debugGUI(void)
{
	// list entities to pick from
	static bool eclose = false;
	const char* classname;

	if (!eclose && ImGui::Begin("Explorer", &eclose)) {
		if (ImGui::TreeNode("gaEntities")) {
			for (auto& mclass : m_entitiesByClass) {
				classname = g_entityClassName[mclass.first];
				if (ImGui::TreeNode(classname)) {
					mclass.second.sort(
						[](gaEntity* a, gaEntity* b) { return a->name() < b->name(); }
					);
					for (auto entity : mclass.second) {
						const std::string& name = entity->name();
						bool old = m_watch[name];
						ImGui::Checkbox(name.c_str(), &m_watch[name]);
						if (old != m_watch[name]) {
							if (m_watch[name]) {
								entity->worldAABB().color(glm::vec3(1.0f, 0.0f, 0.0f));
							}
							else {
								entity->worldAABB().color(glm::vec3(1.0f, 1.0f, 1.0f));
							}
						}
					}
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
		ImGui::End();

	}
	// display entities monitored
	ImGui::Begin("Inspector");
	for (auto& watch : m_watch) {
		if (watch.second) {
			auto& entities = m_entities[watch.first];
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
				// only show messages for the monitored entities
				if (m_watch[message->m_server] || m_watch[message->m_client]) {
					message->debugGUI();
				}
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
			// only show messages for the monitored entities
			if (m_watch[message.m_server] || m_watch[message.m_client]) {
				message.debugGUI();
			}
		}
		ImGui::EndTable();
	}
	ImGui::End();
}

World::~World()
{
}
