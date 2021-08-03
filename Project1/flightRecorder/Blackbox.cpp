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
	int messages = g_gaWorld.m_queue.size();
	int data_size = sizeof(bufferMessages) + messages * sizeof(flightRecorder::Message);

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

	/**
	 * save all messages in the buffer
	 */
	gaMessage* message;
	for (int i = 0; i < messages - 1; i++) {
		message = g_gaWorld.m_queue[i];
		message->recordState(&bMessages->messages[i]);
	}
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
	int data_size = sizeof(bufferEntities);
	for (auto entry : g_gaWorld.m_entities) {
		for (auto entity : entry.second) {
			data_size += entity->recordSize();
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
	for (auto entry : g_gaWorld.m_entities) {
		for (auto entity : entry.second) {
			entity->recordState(p);
			p += entity->recordSize();
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
	int objects = g_gaWorld.m_physic.m_ballistics.size();
	int data_size = sizeof(bufferPhysics) + objects * sizeof(flightRecorder::Ballistic);

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
 *
 */
void flightRecorder::Blackbox::recordState(void)
{
	recordMessages();
	recordEntities();
	recordPhysics();

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
 *
 */
void flightRecorder::Blackbox::saveStates(void)
{
	std::ofstream myfile;
	myfile.open("D:/dev/Project1/records.bin", std::ios::out | std::ios::binary);

	myfile.write((char*)&m_len, sizeof(m_len));

	for (int i = 0, p = m_last; i < m_len; i++)
	{
		bufferMessages* messages = m_messages[p];
		myfile.write((char*)&messages->data_size, sizeof(messages->data_size));
		myfile.write((char*)messages, messages->data_size);
		p++;
		if (p >= MAXIMUM_RECORDS) {
			p = 0;
		}
	}

	for (int i = 0, p = m_last; i < m_len; i++)
	{
		bufferEntities* entities = m_entities[p];
		myfile.write((char*)&entities->buffer_size, sizeof(entities->buffer_size));
		myfile.write((char*)entities, entities->buffer_size);
		p++;
		if (p >= MAXIMUM_RECORDS) {
			p = 0;
		}
	}

	for (int i = 0, p = m_last; i < m_len; i++)
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
	for (int i = 0, p = m_last; i < m_len; i++)
	{
		if (m_messages[i])  { free(m_messages[i]);	m_messages[i] = nullptr; }
		if (m_entities[i])  { free(m_entities[i]);	m_entities[i] = nullptr; }
		if (m_ballistics[i]){ free(m_ballistics[i]);	m_ballistics[i] = nullptr; }
	}

	myfile.open("D:/dev/Project1/records.bin", std::ios::in | std::ios::binary);

	//start loading the number of records
	int data_size;
	myfile.read((char*)&m_len, sizeof(m_len));

	// load the messages
	for (int i = 0, p = m_last; i < m_len; i++) {
		myfile.read((char*)&data_size, sizeof(data_size));
		m_messages[i] = (bufferMessages*)malloc(data_size);
		myfile.read((char*)m_messages[i], data_size);
	}

	// load the entities
	for (int i = 0, p = m_last; i < m_len; i++) {
		myfile.read((char*)&data_size, sizeof(data_size));
		m_entities[i] = (bufferEntities*)malloc(data_size);
		myfile.read((char*)m_entities[i], data_size);
	}

	// load the ballistics objects
	for (int i = 0, p = m_last; i < m_len; i++) {
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
void flightRecorder::Blackbox::setState(int frame)
{
	// reset the messages and reload from the records
	g_gaWorld.clearQueue();
	bufferMessages* bMessages = m_messages[frame];
	if (bMessages == nullptr) {
		return;
	}

	flightRecorder::Message* record = &bMessages->messages[0];
	for (auto i = 0; i < bMessages->size; i++) {
		g_gaWorld.sendMessage(record);
	}

	// build a list of the entities in the save
	bufferEntities* bEntities= m_entities[frame];
	std::map<std::string, flightRecorder::Entity*> entities;
	int size = bEntities->size;
	flightRecorder::Entity* rEntity;
	char* p = &bEntities->data[0];
	for (auto i = 0; i < size; i++) {
		rEntity = (flightRecorder::Entity*)p;

		entities[rEntity->name] = rEntity;

		p += rEntity->size;
	}

	//remove entities from the world that are not in the save
	for (auto &entity : g_gaWorld.m_entities) {
		if (entities.count(entity.first) == 0) {
			g_gaWorld.m_entities.erase(entity.first);
		}
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
		}
	}

	// reload the ballistic engine
	g_gaWorld.m_physic.m_ballistics.clear();
	bufferPhysics* bPhysics= m_ballistics[frame];
	for (auto i = 0; i < bPhysics->size; i++) {
		g_gaWorld.m_physic.loadState(&bPhysics->objects[i]);
	}
}

/*/
 * display the flight recorder data on the debugger
 */
void flightRecorder::Blackbox::debugGUI(void)
{
	ImGui::BeginGroup();
	ImGui::Text("flightRecorder v2");
	ImGui::SameLine(); if (ImGui::Button("Load")) {
		// load a flight recorder and position as frame 0
		loadStates();
		setState(0);
	}
	ImGui::SameLine(); if (ImGui::Button("Save")) {
		saveStates();
	}
	if (m_len > 0) {
		ImGui::SameLine(); if (ImGui::Button(">")) {
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
			setState(convertFrame);

			// set the debug camera based on the player position
			//m_control->translateCamera(
			//	dark->m_player->position()
			//	);
		}

		if (m_currentFrame < m_len) {
			// display frame by frame up to the last frame
			ImGui::SameLine(); if (ImGui::Button(">>")) {
				g_gaWorld.run();

				m_replay = true;
			}
		}
	}
	ImGui::EndGroup();
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

