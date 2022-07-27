#pragma once

#include <list>
#include <map>

#define MAXIMUM_RECORDS 300
#include "Message.h"
#include "Entity.h"
#include "Ballistic.h"


namespace flightRecorder {
	/**
	 *
	 */
	struct bufferMessages {
		uint32_t data_size;	// size in bytes of the buffer
		uint32_t size;		// total number of entries in the buffer (start of frame messages)
		uint32_t current;	// current number of messages (up to size)
		flightRecorder::Message messages[1];
	};

	/**
	 *
	 */
	struct bufferEntities {
		uint32_t buffer_size;	// size in bytes of the buffer
		uint32_t data_size;		// number of used bytes in the buffer
		uint32_t size;			// number of entries in the buffer
		char data[1];		// start of the data
	};

	/**
	 *
	*/
	struct bufferPhysics {
		uint32_t data_size;	// size in bytes of the buffer
		uint32_t size;		// number of entries in the buffer
		flightRecorder::Ballistic objects[1];
	};

	/**
	 *
	 */
	class Blackbox {
		// creation function for classes of entities
		std::map<std::string, void* (*)(void *)> m_callbacks;

		// circular buffers
		struct bufferEntities* m_entities[MAXIMUM_RECORDS];
		struct bufferMessages* m_messages[MAXIMUM_RECORDS];
		struct bufferPhysics* m_ballistics[MAXIMUM_RECORDS];
		uint32_t			  m_frames[MAXIMUM_RECORDS];
		std::list<gaMessage>	m_inframe_messages[MAXIMUM_RECORDS];

		// recording
		uint32_t m_first = 0;		// first record (cycle once reaching MAXIMUM_RECORDS
		uint32_t m_last = 0;		// last record
		uint32_t m_len = 0;			// number of records
		uint32_t m_current = 0;		// current position

		//replay
		bool m_replay = false;
		int32_t m_currentFrame = -1;

		void recordMessages(void);
		void recordEntities(void);
		void recordPhysics(void);

	public:
		Blackbox();

		void registerClass(const std::string& className, void* (*func)(void*));	// register the callback creation function for the class name

		void recordState(void);
		void recordMessage(gaMessage*);	// record messages on the fly (# over number of start of frame, move to _inframe_)
		void saveStates(void);
		void loadStates(void);
		void setFrame(int frame);	// reload frame #
		void nextFrame(void);		// reload next frame
		void previousFrame(void);	// reload previous frame

		inline int len(void) { return m_len; }

		bool debugGUI(bool b);		// display the flight recorder data on the debugger
		void debugGUImessages(bool b);	// display messages from the queue
		void debugGUIinframe(bool b);	// display messages inframe

		~Blackbox();
	};
}

extern flightRecorder::Blackbox g_Blackbox;
