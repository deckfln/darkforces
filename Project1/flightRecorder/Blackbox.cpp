#include "Blackbox.h"

#include <iostream>
#include <fstream>

#include "../gaEngine/World.h"
#include "../gaEngine/gaEntity.h"
#include "Message.h"
#include "Entity.h"

/**
 * global flight recorder
 */
flightRecorder::Blackbox g_Blackbox;

flightRecorder::Blackbox::Blackbox()
{
}

/**
 *
 */
struct bufferMessages {
	int data_size;	// size in bytes of the buffer
	int size;		// number of entries in the buffer
	flightRecorder::Message messages[0];
};

void flightRecorder::Blackbox::recordMessages()
{
	/**
	 * get a buffer for messages
	 */
	bufferMessages* bMessages = nullptr;
	int messages = g_gaWorld.m_queue.size();
	int data_size = sizeof(bufferMessages) + messages * sizeof(flightRecorder::Message);

	// we may need more memory than the previous allocated buffer
	bMessages = (bufferMessages*)m_messages[m_last];

	if (bMessages == nullptr || m_len == 0) {
		// at begining, build from scratch
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
struct bufferEntities {
	int buffer_size;	// size in bytes of the buffer
	int data_size;		// number of used bytes in the buffer
	int size;			// number of entries in the buffer
	char data[0];		// start of the data
};

void flightRecorder::Blackbox::recordEntities()
{
	//get a buffer for messages
	bufferEntities* bEntities = nullptr;
	int entities = g_gaWorld.m_entities.size();

	// compute the needed space
	int data_size = sizeof(bufferMessages);
	for (auto entry : g_gaWorld.m_entities) {
		for (auto entity : entry.second) {
			data_size += entity->recordSize();
		}
	}

	// we may need more memory than the previous allocated buffer
	bEntities = (bufferEntities*)m_entities[m_last];

	if (bEntities == nullptr || m_len == 0) {
		// at begining, build from scratch
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
void flightRecorder::Blackbox::recordState(void)
{
	recordMessages();
	recordEntities();

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
	myfile.open("records.bin", std::ios::out | std::ios::binary);

	myfile.write((char*)&m_len, sizeof(m_len));

	for (int i = 0, p = m_last; i < m_len; i++)
	{
		bufferMessages* messages = (bufferMessages *)m_messages[p];
		myfile.write((char*)&messages->data_size, sizeof(messages->data_size));
		myfile.write((char*)messages, messages->data_size);
		p++;
		if (p >= MAXIMUM_RECORDS) {
			p = 0;
		}
	}

	for (int i = 0, p = m_last; i < m_len; i++)
	{
		bufferEntities* entities = (bufferEntities*)m_entities[p];
		myfile.write((char*)&entities->buffer_size, sizeof(entities->buffer_size));
		myfile.write((char*)entities, entities->buffer_size);
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
flightRecorder::Blackbox::~Blackbox()
{
}

