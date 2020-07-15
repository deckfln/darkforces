#pragma once

#include <queue>
#include <map>
#include <list>

#include "../framework/fwAABBox.h"
#include "../framework/math/fwCylinder.h"

#include "gaCollisionPoint.h"
#include "gaMessage.h"
#include "../gaEngine/gaModel.h"

class gaEntity;
class fwScene;
class fwMesh;
class dfSuperSector;
class dfSprites;

class gaWorld
{
	std::queue<gaMessage*> m_queue;
	std::queue<gaMessage*> m_for_next_frame;
	std::map<std::string, std::list<gaEntity*>> m_entities;
	std::vector<dfSuperSector*> m_sectors;
	dfSprites* m_sprites;							// sprites manager
	std::map<std::string, GameEngine::gaModel*> m_models;	// list of models (images, 3D objects ...)

	bool m_timer = true;							// pass DF_MESSAGE_TIMER event
	fwScene* m_scene;								// current scene on screen;

public:
	gaWorld(void);

	void scene(fwScene* scene) { m_scene = scene; };

	void addClient(gaEntity* client);					// add a spirit entity
	void removeClient(gaEntity* client);				// remove a spirit entity
	void add2scene(gaEntity* client);					// add an entiuty mesh (if it has one) to the current scene
	void addSector(dfSuperSector* client);				// add a game sector

	void addModel(GameEngine::gaModel* model);							// add a new model to the world
	GameEngine::gaModel* getModel(const std::string& name);				// get a model from the world
	bool removeModel(const std::string& name);							// remove a model from the world
	void getModelsByClass(
		uint32_t myclass,
		std::list<GameEngine::gaModel*>& r );			// return all models of a specific class

	void spritesManager(dfSprites* sprites);			// add the sprites manager

	gaMessage* sendMessage(const std::string& from, 
		const std::string& to, 
		int action, 
		int value, 
		void *extra);									// send message for immediate action

	gaMessage* sendMessageDelayed(const std::string& from,
		const std::string& to,
		int action,
		int value,
		void* extra);									// send message for next frame

	gaEntity* getEntity(const std::string& name);
	dfSprites* spritesManager(void) {
		return m_sprites;
	}

	void findAABBCollision(fwAABBox& box, 
		std::list<gaEntity*>& collisions, 
		std::list<dfSuperSector*>& sectors);			// find colliding entities & sectors AABB
	virtual bool checkCollision(
		gaEntity* source,
		fwCylinder& bounding,
		glm::vec3& direction,
		std::list<gaCollisionPoint>& collisions);		// extended collision test after a sucessfull AABB collision

	void push(gaMessage* message);
	void pushForNextFrame(gaMessage* message);
	void process(time_t delta);
	void suspendTimer(void);
	~gaWorld();
};

extern gaWorld g_gaWorld;