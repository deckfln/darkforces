#pragma once

#include <queue>
#include <map>
#include <list>

#include "../framework/fwAABBox.h"
#include "../framework/math/fwCylinder.h"

#include "gaCollisionPoint.h"
#include "gaMessage.h"

class gaEntity;
class fwScene;
class fwMesh;

class gaWorld
{
	std::queue<gaMessage*> m_queue;
	std::queue<gaMessage*> m_for_next_frame;
	std::map<std::string, std::list<gaEntity*>> m_entities;

	bool m_timer = true;		// pass DF_MESSAGE_TIMER event
	fwScene* m_scene;			// current scene on screen;

public:
	gaWorld(void);

	void scene(fwScene* scene) { m_scene = scene; };

	void addClient(gaEntity* client);		// add a spirit entity
	void removeClient(gaEntity* client);	// remove a spirit entity
	void add2scene(fwMesh* client);			// add a mesh to the current scene

	gaMessage* sendMessage(const std::string& from, const std::string& to, int action, int value, void *extra);	
	gaEntity* getEntity(const std::string& name);

	void findAABBCollision(fwAABBox& box, std::list<gaEntity*>& collisions);	// find colliding entities AABB
	virtual bool checkCollision(
		gaEntity* source,
		fwCylinder& bounding,
		glm::vec3& direction,
		std::list<gaCollisionPoint>& collisions);								// extended collision test after a sucessfull AABB collision

	void push(gaMessage* message);
	void pushForNextFrame(gaMessage* message);
	void process(time_t delta);
	void suspendTimer(void);
	~gaWorld();
};

extern gaWorld g_gaWorld;