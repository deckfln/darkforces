#pragma once

#include <queue>
#include <map>
#include <list>

#include "../gaEngine/gaEntity.h"
#include "dfMessage.h"

class dfMessageBus
{
	std::queue<dfMessage*> m_queue;
	std::queue<dfMessage*> m_for_next_frame;
	std::map<std::string, std::list<gaEntity*>> m_entities;

	bool m_timer = true;		// pass DF_MESSAGE_TIMER event

public:
	dfMessageBus(void);
	void addClient(dfMessageClient* client);
	void addClient(gaEntity* client);
	void removeClient(gaEntity* client);

	gaEntity* getEntity(const std::string& name);

	void findAABBCollision(fwAABBox& box, std::list<gaEntity*>& collisions);	// find colliding entities AABB

	void push(dfMessage* message);
	void pushForNextFrame(dfMessage* message);
	void process(time_t delta);
	void suspendTimer(void);
};

extern dfMessageBus g_MessageBus;