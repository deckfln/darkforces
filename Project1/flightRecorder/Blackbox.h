#pragma once

#define MAXIMUM_RECORDS 300

namespace flightRecorder {
	class Blackbox {
		void* m_entities[MAXIMUM_RECORDS];
		void* m_messages[MAXIMUM_RECORDS];

		int m_first = 0;	// first record (cycle once reaching MAXIMUM_RECORDS
		int m_last = 0;		// last record
		int m_len = 0;		// number of records

		void recordMessages();
		void recordEntities();

	public:
		Blackbox();
		void recordState(void);
		void saveStates(void);
		~Blackbox();
	};
}

extern flightRecorder::Blackbox g_Blackbox;
