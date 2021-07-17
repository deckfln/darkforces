#pragma once

#include <queue>
#include <map>
#include <list>

#include "../framework/fwAABBox.h"
#include "../framework/math/fwCylinder.h"

#include "gaCollisionPoint.h"
#include "gaMessage.h"
#include "Model.h"
#include "Physics.h"
#include "../flightRecorder/Blackbox.h"

class gaEntity;
class fwScene;
class fwMesh;
class dfSuperSector;
class dfSprites;

namespace GameEngine {
	class World
	{
		std::deque<gaMessage*> m_queue;
		std::deque<gaMessage*> m_for_next_frame;
		std::map<std::string, std::list<gaEntity*>> m_entities;
		std::vector<dfSuperSector*> m_sectors;
		dfSprites* m_sprites = nullptr;							// sprites manager
		std::map<std::string, GameEngine::Model*> m_models;	// list of models (images, 3D objects ...)
		std::map<std::string, void*> m_registry;				// list of objects

		bool m_timer = true;								// pass DF_MESSAGE_TIMER event
		fwScene* m_scene;									// current scene on screen;
		GameEngine::Physics m_physic;						// physic/collision engine

		friend GameEngine::Physics;							// physic engine has direct access to world data
		friend flightRecorder::Blackbox;					// flight recorder has direct access to everything

	public:
		World(void);

		void scene(fwScene* scene);							// register the scene

		void addClient(gaEntity* client);					// add a spirit entity
		void removeClient(gaEntity* client);				// remove a spirit entity
		void add2scene(gaEntity* client);					// add an entity mesh (if it has one) to the current scene
		void add2scene(fwMesh* mesh);						// add a mesh 
		void remove2scene(fwMesh* mesh);					// remove a mesh
		void addSector(dfSuperSector* client);				// add a game sector
		dfSuperSector* findSector(dfSuperSector* cached,
			const glm::vec3& position);						// find the super sector the position is inside
		void set(const std::string& name, void* object);	// add a new generic object
		void* get(const std::string& name);					// retrieve a generic object

		void addModel(GameEngine::Model* model);							// add a new model to the world
		GameEngine::Model* getModel(const std::string& name);				// get a model from the world
		bool removeModel(const std::string& name);							// remove a model from the world
		void getModelsByClass(
			uint32_t myclass,
			std::list<GameEngine::Model*>& r);			// return all models of a specific class

		void spritesManager(dfSprites* sprites);			// add the sprites manager

		gaMessage* sendMessage(const std::string& from,
			const std::string& to,
			int action,
			int value,
			void* extra);									// send message an the queue

		gaMessage* sendMessage(const std::string& from,
			const std::string& to,
			int action,
			float value,
			void* extra);									// send message an the queue

		gaMessage* sendMessage(const std::string& from,
			const std::string& to,
			int action,
			const glm::vec3& value,
			void* extra);									// send message an the queue

		gaMessage* sendMessageDelayed(const std::string& from,
			const std::string& to,
			int action,
			int value,
			void* extra);									// send message for next frame

		gaMessage* sendImmediateMessage(const std::string& from,
			const std::string& to,
			int action,
			int value,
			void* extra);									// send message for immediate dispatch

		gaEntity* getEntity(const std::string& name);
		dfSprites* spritesManager(void) {
			return m_sprites;
		}

		void findAABBCollision(const fwAABBox& box,
			std::list<gaEntity*>& collisions,
			std::list<dfSuperSector*>& sectors,
			gaEntity* source);								// find colliding entities & sectors AABB
		virtual bool checkCollision(
			gaEntity* source,
			fwCylinder& bounding,
			glm::vec3& direction,
			std::list<gaCollisionPoint>& collisions);		// extended collision test after a successful AABB collision

		void push(gaMessage* message);
		void pushForNextFrame(gaMessage* message);
		void process(time_t delta);
		void suspendTimer(void);
		void renderGUI(void);								// render the imGUI debug
		~World();
	};
}

extern GameEngine::World g_gaWorld;

