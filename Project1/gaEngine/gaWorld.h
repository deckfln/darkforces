#pragma once

#include <queue>
#include <map>
#include <list>

#include "gaEntity.h"
#include "gaMessage.h"

class gaWorld
{
	std::queue<gaMessage*> m_queue;
	std::queue<gaMessage*> m_for_next_frame;
	std::map<std::string, std::list<gaEntity*>> m_entities;

	bool m_timer = true;		// pass DF_MESSAGE_TIMER event

public:
	gaWorld(void);
	void addClient(gaEntity* client);
	void removeClient(gaEntity* client);

	gaMessage* sendMessage(const std::string& from, const std::string& to, int action, int value, void *extra);	
	gaEntity* getEntity(const std::string& name);

	void findAABBCollision(fwAABBox& box, std::list<gaEntity*>& collisions);	// find colliding entities AABB

	void push(gaMessage* message);
	void pushForNextFrame(gaMessage* message);
	void process(time_t delta);
	void suspendTimer(void);
	~gaWorld();
};

extern gaWorld g_gaWorld;