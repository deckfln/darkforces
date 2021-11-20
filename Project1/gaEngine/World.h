#pragma once

#include <queue>
#include <map>
#include <list>

#include "../framework/fwAABBox.h"
#include "../framework/math/fwCylinder.h"

#include "Collider.h"
#include "gaCollisionPoint.h"
#include "gaMessage.h"
#include "Model.h"
#include "Physics.h"
#include "../flightRecorder/Blackbox.h"
#include "../flightRecorder/Message.h"

class gaEntity;
class fwScene;
class fwMesh;
class dfSuperSector;
class dfSprites;
class alSound;

namespace GameEngine {
	struct Alarm {
		uint32_t m_id=0;
		gaEntity* m_entity;
		uint32_t m_message;
		time_t m_delay;

		bool operator==(const struct Alarm& a) const
		{
			return (a.m_id == this->m_id);
		}

		inline Alarm(gaEntity* entity, time_t delay, uint32_t message = gaMessage::Action::ALARM):
			m_entity(entity),
			m_delay(delay),
			m_message(message)
		{
		}
	};

	class World
	{
		bool m_run=true;									// run or suspend the engine
		uint32_t m_frame = 0;								// current frame
		std::deque<gaMessage*> m_queue;
		std::deque<gaMessage*> m_for_next_frame;

		std::map<std::string, std::list<gaEntity*>> m_entities;
		std::map<uint32_t, std::list<gaEntity*>> m_entitiesByClass;

		std::list<gaEntity*> m_timers;						// entities that registered to receive timer events
		std::list<GameEngine::Alarm> m_alarms;				// entities that registered to receive alarm events
		std::map<uint32_t, gaEntity*> m_views;				// entities that registered to receive view events
		std::map<uint32_t, gaEntity*> m_hear;				// entities that registered to receive audio events

		std::vector<dfSuperSector*> m_sectors;
		dfSprites* m_sprites = nullptr;						// sprites manager
		std::map<std::string, GameEngine::Model*> m_models;	// list of models (images, 3D objects ...)
		std::map<std::string, void*> m_registry;				// list of objects

		bool m_timer = true;								// pass DF_MESSAGE_TIMER event
		fwScene* m_scene;									// current scene on screen;

		friend GameEngine::Physics;							// physic engine has direct access to world data
		friend flightRecorder::Blackbox;					// flight recorder has direct access to everything

		std::map<std::string, bool>	m_watch;				// keep a list of entities to display in the debugger

		void checkSoundPerceptions(gaEntity* source, const glm::vec3& p, alSound* sound);			// parse all listening entity to check if they can hear a PLAY_SOUND message

	public:
		World(void);

		void run(void);										// run the game engine
		void suspend(void);									// suspend the game engine
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

		gaMessage* sendMessage(gaMessage* msg);
		gaMessage* sendMessage(const std::string& from,
			const std::string& to,
			int action,
			int value,
			void* extra);									// send message an the queue

		gaMessage* sendMessage(const std::string& from,
			const std::string& to,
			int action,
			int value,
			float fvalue,
			void* extra);									

		gaMessage* sendMessage(const std::string& from,
			const std::string& to,
			int action,
			int value,
			const glm::vec3& v3value,
			void* extra);

		gaMessage* sendMessage(const std::string& from,
			const std::string& to,
			int action,
			int value,
			float fvalue,
			const glm::vec3& v3value,
			void* extra);

		gaMessage* sendMessage(const std::string& from,
			const std::string& to,
			int action,
			float value,
			void* extra);									

		gaMessage* sendMessage(const std::string& from,
			const std::string& to,
			int action,
			const glm::vec3& value,
			void* extra);									// send message an the queue

		gaMessage* sendMessage(flightRecorder::Message *message);// send message an the queue

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

		bool deleteMessage(uint32_t id);					// delete a previously submitted message
		void deleteMessages(gaEntity*);						// delete all messages for that entity

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
		void process(time_t delta, bool force=false);		// process messages on the queue
		void suspendTimer(void);							// stop the engine
		void clearQueue(void);								// clear the message queue
		inline int queueLen(void) { return m_queue.size(); }// number of messages on the queue
		inline int frame(void) { return m_frame; }			// number of messages on the queue
		void update(void);									// force an update of the world

		bool lineOfSight(gaEntity* source, gaEntity* target); // test the line of the sight between 2 entities

		bool intersectWithEntity(
			uint32_t componentID,
			GameEngine::Collider& segment,
			std::vector<gaEntity*>& collision);				// find all entities intersecting with the segment
		bool intersectWithEntity(
			Framework::Segment& segment,
			std::vector<gaEntity*>& entCollisions);


		void getEntitiesWithComponents(uint32_t componentID,
			std::vector<gaEntity*>& entities);				// return all entities with a special components
		bool getEntities(uint32_t type,
			std::vector<gaEntity*>& entities);				// return all entities of that type

		// Register entities for Timer events
		void registerTimerEvents(gaEntity*, bool b);		// (de)register an entity to receive timer events

		// (de)register entities for Alarm events
		uint32_t registerAlarmEvent(Alarm& alarm);			// register an entity to receive alarm event
		bool cancelAlarmEvent(uint32_t id);					// cancel a programmed alarm

		// (de)register entities for visual perceptions
		void registerViewEvents(gaEntity* entity);
		void deRegisterViewEvents(gaEntity* entity);
		void registerHearEvents(gaEntity* entity);
		void deRegisterHearEvents(gaEntity* entity);
		void checkPerceptions(void);

		// debugger
		void debugGUI(void);								// render the imGUI debug
		void debugGUImessages(std::list<gaMessage>& l);		// render the imGUI debug messages

		~World();
	};
}

extern GameEngine::World g_gaWorld;

