#pragma once

#include "gaMessage.h"

class gaEntity;

class gaComponent
{
protected:
	bool m_active = true;								// does the component handle requests ?
	gaEntity* m_entity = nullptr;						// parent entity
	uint32_t m_id = 0;									// position of the component in the entity components list
	int m_type = NONE;

public:
	enum {
		NONE = 0,
		MESH,
		COLLIDER,
		CONTROLER,
		SOUND,
		ActiveProbe,
		Controller,
		PathFinding,
		BehaviorTree,
		AIPerception,
		Actor,
		Listener,
		Inventory
	};

	gaComponent(int m_type);

	// getter/setter
	inline bool is(int type) { return type == m_type; };
	inline void parent(gaEntity* parent) { m_entity = parent; };
	inline gaEntity* entity(void) { return m_entity; };
	inline uint32_t entityID(void);
	inline bool active(void) { return m_active; };
	inline void active(bool b) { m_active = b; };

	virtual void dispatchMessage(gaMessage* message) {};	// let a component deal with a situation

	// flight recorder status
	virtual inline uint32_t recordSize(void) { return sizeof(uint32_t); };// size of the component record
	virtual uint32_t recordState(void* record);				// save the component state in a record
	virtual inline uint32_t loadState(void* record) { return sizeof(uint32_t); };	// reload a component state from a record

	virtual inline void debugGUIinline(void) {};			// display the component in the debugger
	~gaComponent();
};