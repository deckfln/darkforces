#include "Blackbox.h"

#include <iostream>
#include <fstream>
#include <map>
#include <imgui.h>

#include "../gaEngine/World.h"
#include "../gaEngine/Physics.h"
#include "../gaEngine/gaMessage.h"

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaActor.h"

#include "Callbacks.h"

/**
 * global flight recorder
 */
flightRecorder::Blackbox g_Blackbox;

/**
 * 	callbacks to create new entities
 */

#define MAXIMUM_CALLBACKS 1024
void* (*g_Callbacks[MAXIMUM_CALLBACKS])(void* record) = { nullptr };

static gaEntity* createEntity(flightRecorder::Entity* record)
{
	int mclass = (int)record->classID;
	if (g_Callbacks[mclass]) {
		return (gaEntity*)(*g_Callbacks[mclass])(record);
	}
	return nullptr;
}

/**
 *
 */
flightRecorder::Blackbox::Blackbox()
{
	init_callbacks();
}

/**
 *
 */
void flightRecorder::Blackbox::recordMessages(void)
{
	/**
	 * get a buffer for messages
	 */
	bufferMessages* bMessages = nullptr;
	uint32_t messages = g_gaWorld.m_queue.size();
	uint32_t data_size = sizeof(bufferMessages) + messages * sizeof(flightRecorder::Message);

	// we may need more memory than the previous allocated buffer
	bMessages = m_messages[m_last];

	if (bMessages == nullptr || m_len == 0) {
		// at beginning, build from scratch
		bMessages = (bufferMessages*)malloc(data_size);
	}
	else {
		if (messages > bMessages->size) {
			free(bMessages);
			bMessages = (bufferMessages*)malloc(data_size);
		}
	}
	m_messages[m_last] = bMessages;

	bMessages->data_size = data_size;
	bMessages->size = messages;
	bMessages->current = 0;

	/**
	 * save all start_of_frame messages in the buffer
	 */
	gaMessage* message;
	for (uint32_t i = 0; i < messages; i++) {
		message = g_gaWorld.m_queue[i];
		message->recordState(&bMessages->messages[i]);
	}

	// empty the inframe message list
	m_inframe_messages[m_last].clear();
}

/**
 *
 */
void flightRecorder::Blackbox::recordEntities(void)
{
	//get a buffer for messages
	bufferEntities* bEntities = nullptr;
	int entities = 0;

	// compute the needed space
	uint32_t data_size = sizeof(bufferEntities);
	for (auto& entry : g_gaWorld.m_entities) {
		for (auto entity : entry.second) {
			data_size += entity->recordSize() + entity->componentsSize();
			entities++;
		}
	}

	// we may need more memory than the previous allocated buffer
	bEntities = m_entities[m_last];

	if (bEntities == nullptr || m_len == 0) {
		// at beginning, build from scratch
		bEntities = (bufferEntities*)malloc(data_size);
		bEntities->buffer_size = data_size;
	}
	else {
		if (data_size > bEntities->buffer_size) {
			free(bEntities);
			bEntities = (bufferEntities*)malloc(data_size);
			bEntities->buffer_size = data_size;
		}
	}
	bEntities->data_size = data_size;
	bEntities->size = entities;
	m_entities[m_last] = bEntities;

	// save each entity at the end of the previous
	char *p = &bEntities->data[0];
	char* c = nullptr;
	for (auto entry : g_gaWorld.m_entities) {
		for (auto entity : entry.second) {
			entity->recordState(p);
			p += entity->recordSize();
			p += entity->recordComponents(p);
		}
	}
}

/**
 * 
 */

void flightRecorder::Blackbox::recordPhysics(void)
{
	/**
	 * get a buffer for messages
	 */
	bufferPhysics* bPhysics = nullptr;
	uint32_t objects = g_gaWorld.m_physic.m_ballistics.size();
	uint32_t data_size = sizeof(bufferPhysics) + objects * sizeof(flightRecorder::Ballistic);

	// we may need more memory than the previous allocated buffer
	bPhysics= m_ballistics[m_last];

	if (bPhysics== nullptr || m_len == 0) {
		// at beginning, build from scratch
		bPhysics = (bufferPhysics*)malloc(data_size);
	}
	else {
		if (objects> bPhysics->size) {
			free(bPhysics);
			bPhysics = (bufferPhysics*)malloc(data_size);
		}
	}
	m_ballistics[m_last] = bPhysics;

	bPhysics->data_size = data_size;
	bPhysics->size = objects;

	/**
	 * save all objects in the buffer
	 */
	GameEngine::Ballistic* object;
	int i = 0;
	for (auto& object: g_gaWorld.m_physic.m_ballistics) {
		g_gaWorld.m_physic.recordState(object.first, &bPhysics->objects[i]);
		i++;
	}
}

/**
 * record the state of g_gaWorld at begining of frame
 */
void flightRecorder::Blackbox::recordState(void)
{
	m_frames[m_last] = g_gaWorld.m_frame;

	recordMessages();
	recordEntities();
	recordPhysics();

	m_current = m_last;

	/*
	 * find the next record(cycle at the end of the recorder)
	 */
	m_last++;

	if (m_len == MAXIMUM_RECORDS-1) {
		if (m_last >= MAXIMUM_RECORDS) {
			m_last = 0;
		}

		m_first++;
		if (m_first >= MAXIMUM_RECORDS) {
			m_first = 0;
		}
	}
	else {
		m_len++;
	}
}

/**
 * record messages on the fly 
 *   up to bEntities.size, do nothing
 *   over size, record in _inframe_ list
 */
void flightRecorder::Blackbox::recordMessage(gaMessage* message)
{
	bufferMessages* bMessages = m_messages[m_current];
	if (bMessages->current > bMessages->size) {
		m_inframe_messages[m_current].push_back(*message);
	}
	else {
		bMessages->current++;
	}
}

/**
 *
 */
void flightRecorder::Blackbox::saveStates(void)
{
	std::ofstream myfile;
	myfile.open("D:/dev/Project1/records.bin", std::ios::out | std::ios::binary);

	myfile.write((char*)&m_len, sizeof(m_len));

	myfile.write((char*)m_frames, sizeof(m_frames));

	for (uint32_t i = 0, p = m_last; i < m_len; i++)
	{
		bufferMessages* messages = m_messages[p];
		myfile.write((char*)&messages->data_size, sizeof(messages->data_size));
		myfile.write((char*)messages, messages->data_size);
		p++;
		if (p >= MAXIMUM_RECORDS) {
			p = 0;
		}
	}

	for (uint32_t i = 0, p = m_last; i < m_len; i++)
	{
		bufferEntities* entities = m_entities[p];
		myfile.write((char*)&entities->buffer_size, sizeof(entities->buffer_size));
		myfile.write((char*)entities, entities->buffer_size);
		p++;
		if (p >= MAXIMUM_RECORDS) {
			p = 0;
		}
	}

	for (uint32_t i = 0, p = m_last; i < m_len; i++)
	{
		bufferPhysics* objects = m_ballistics[p];
		myfile.write((char*)&objects->data_size, sizeof(objects->data_size));
		myfile.write((char*)objects, objects->data_size);
		p++;
		if (p >= MAXIMUM_RECORDS) {
			p = 0;
		}
	}

	myfile.close();
}

/**
 *
 */
void flightRecorder::Blackbox::loadStates(void)
{
	std::ifstream myfile;

	// start freeing all previous records
	for (uint32_t i = 0, p = m_last; i < m_len; i++)
	{
		if (m_messages[i])  { free(m_messages[i]);	m_messages[i] = nullptr; }
		if (m_entities[i])  { free(m_entities[i]);	m_entities[i] = nullptr; }
		if (m_ballistics[i]){ free(m_ballistics[i]);	m_ballistics[i] = nullptr; }
	}

	myfile.open("D:/dev/Project1/records.bin", std::ios::in | std::ios::binary);

	//start loading the number of records
	int data_size;
	myfile.read((char*)&m_len, sizeof(m_len));

	myfile.read((char*)m_frames, sizeof(m_frames));

	// load the messages
	for (uint32_t i = 0, p = m_last; i < m_len; i++) {
		myfile.read((char*)&data_size, sizeof(data_size));
		m_messages[i] = (bufferMessages*)malloc(data_size);
		myfile.read((char*)m_messages[i], data_size);
	}

	// load the entities
	for (uint32_t i = 0, p = m_last; i < m_len; i++) {
		myfile.read((char*)&data_size, sizeof(data_size));
		m_entities[i] = (bufferEntities*)malloc(data_size);
		myfile.read((char*)m_entities[i], data_size);
	}

	// load the ballistics objects
	for (uint32_t i = 0, p = m_last; i < m_len; i++) {
		myfile.read((char*)&data_size, sizeof(data_size));
		m_ballistics[i] = (bufferPhysics*)malloc(data_size);
		myfile.read((char*)m_ballistics[i], data_size);
	}

	m_first = 0;
	m_last = m_len;
}

/**
 *
 */
void flightRecorder::Blackbox::setFrame(int frame)
{
	// reset the messages and reload from the records
	g_gaWorld.clearQueue();
	g_gaWorld.m_frame = m_frames[frame];

	bufferMessages* bMessages = m_messages[frame];
	if (bMessages == nullptr) {
		return;
	}

	flightRecorder::Message* record = &bMessages->messages[0];
	for (uint32_t i = 0; i < bMessages->size; i++) {
		g_gaWorld.sendMessage(record);
		record++;
	}

	// build a list of the entities in the save
	bufferEntities* bEntities= m_entities[frame];
	std::map<std::string, flightRecorder::Entity*> entities;
	std::map<std::string, void*> components;

	uint32_t size = bEntities->size;
	flightRecorder::Entity* rEntity;
	uint32_t* component=nullptr;
	char* p = &bEntities->data[0];
	for (uint32_t i = 0; i < size; i++) {
		rEntity = (flightRecorder::Entity*)p;

		entities[rEntity->name] = rEntity;

		p += rEntity->size;

		// start of the components
		components[rEntity->name] = p;
		for (uint32_t i = 0; i < rEntity->nbComponents; i++) {
			component = (uint32_t*)p;
			p += *component;		// move to next component 
		}
	}

	//remove entities from the world that are not in the save
	std::list<std::string> to_delete;
	for (auto &entity : g_gaWorld.m_entities) {
		if (entities.count(entity.first) == 0) {
			to_delete.push_back(entity.first);
		}
	}
	for (auto entity : to_delete) {
		g_gaWorld.m_entities.erase(entity);
	}

	// add entities to the world that are not in the save
	gaEntity* child;
	for (auto& entity : entities) {
		if (g_gaWorld.m_entities.count(entity.first) == 0) {
			child = createEntity(entity.second);
			g_gaWorld.m_entities[entity.first].push_back(child);
		}
	}

	// and reload their states
	for (auto &entry : g_gaWorld.m_entities) {
		for (auto &ent : entry.second) {
			ent->loadState(entities[entry.first]);
			ent->loadComponents(components[entry.first]);
		}
	}

	// reload the ballistic engine
	g_gaWorld.m_physic.m_ballistics.clear();
	bufferPhysics* bPhysics= m_ballistics[frame];
	for (uint32_t i = 0; i < bPhysics->size; i++) {
		g_gaWorld.m_physic.loadState(&bPhysics->objects[i]);
	}

	// and update the world
	g_gaWorld.update();
}

/*
 * reload next frame
 */
void flightRecorder::Blackbox::nextFrame(void)
{
	if (m_currentFrame < m_len - 1) {
		m_currentFrame++;
		// convert the absolute frame number to the circular buffer
		int convertFrame = (m_first + m_currentFrame) % m_len;

		// Set the game state
		setFrame(convertFrame);
	}
}

/**
 * reload previous frame
 */
void flightRecorder::Blackbox::previousFrame(void)
{
	if (m_currentFrame > 0) {
		m_currentFrame--;
		// convert the absolute frame number to the circular buffer
		int convertFrame = (m_first + m_currentFrame) % m_len;

		// Set the game state
		setFrame(convertFrame);
	}
}

/*/
 * display the flight recorder data on the debugger
 */
void flightRecorder::Blackbox::debugGUI(void)
{
	ImGui::Begin("flightRecorder v2");
	if (ImGui::Button("Load")) {
		// load a flight recorder and position as frame 0
		loadStates();
		setFrame(0);
	}
	ImGui::SameLine(); if (ImGui::Button("Save")) {
		saveStates();
	}
	if (m_len > 0) {
		if (ImGui::Button(">")) {
			if (!m_replay) {
				m_replay = true;
			}
			g_gaWorld.run();
		}

		if (m_currentFrame < 0) {
			m_currentFrame = m_len - 1;
		}

		int old_frame = m_currentFrame;
		ImGui::SameLine(); ImGui::SliderInt("frame", &m_currentFrame, 0, m_len);
		if (old_frame != m_currentFrame) {
			// convert the absolute frame number to the circular buffer
			int convertFrame = (m_first + m_currentFrame) % m_len;

			// Set the game state
			setFrame(convertFrame);

			// set the debug camera based on the player position
			//m_control->translateCamera(
			//	dark->m_player->position()
			//	);
		}

		if (m_currentFrame > 0) {
			// display frame by frame up to the last frame
			ImGui::SameLine(); if (ImGui::Button("<<")) {
				previousFrame();
			}
		}

		if (m_currentFrame < m_len) {
			// display frame by frame up to the last frame
			ImGui::SameLine(); if (ImGui::Button(">>")) {
				nextFrame();
			}
		}
	}
	ImGui::End();
}

/**
 * display messages inframe
 */
void flightRecorder::Blackbox::debugGUIinframe(void)
{
	// convert the absolute frame number to the circular buffer
	int convertFrame = (m_first + m_currentFrame) % m_len;
	g_gaWorld.debugGUImessages(m_inframe_messages[convertFrame]);
}

/**
 *
 */
flightRecorder::Blackbox::~Blackbox()
{
}


/**
 * Create a polymorphic entity
 */
static gaEntity* createEntity1(flightRecorder::Entity* record)
{
	gaEntity* entity = nullptr;
	switch (record->classID) {
	case flightRecorder::TYPE::ENTITY:
		entity = new gaEntity(record);
		break;
	case flightRecorder::TYPE::ENTITY_ACTOR:
		entity = new gaActor(record);
		break;
	}
	return entity;
}

