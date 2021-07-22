#pragma once

#define MAXIMUM_RECORDS 300

#include "Message.h"
#include "Entity.h"
#include "Ballistic.h"

namespace flightRecorder {
	/**
	 *
	 */
	struct bufferMessages {
		int data_size;	// size in bytes of the buffer
		int size;		// number of entries in the buffer
		flightRecorder::Message messages[1];
	};

	/**
	 *
	 */
	struct bufferEntities {
		int buffer_size;	// size in bytes of the buffer
		int data_size;		// number of used bytes in the buffer
		int size;			// number of entries in the buffer
		char data[1];		// start of the data
	};

	/**
	 *
	*/
	struct bufferPhysics {
		int data_size;	// size in bytes of the buffer
		int size;		// number of entries in the buffer
		flightRecorder::Ballistic objects[1];
	};

	/**
	 *
	 */
	class Blackbox {
		struct bufferEntities* m_entities[MAXIMUM_RECORDS];
		struct bufferMessages* m_messages[MAXIMUM_RECORDS];
		struct bufferPhysics* m_ballistics[MAXIMUM_RECORDS];

		int m_first = 0;	// first record (cycle once reaching MAXIMUM_RECORDS
		int m_last = 0;		// last record
		int m_len = 0;		// number of records

		void recordMessages(void);
		void recordEntities(void);
		void recordPhysics(void);

	public:
		Blackbox();
		void recordState(void);
		void saveStates(void);
		void loadStates(void);
		void setState(int frame);

		inline int len(void) { return m_len; }

		~Blackbox();
	};
}

extern flightRecorder::Blackbox g_Blackbox;
