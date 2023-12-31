#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../framework/fwObject3D.h"
#include "../framework/fwAABBox.h"
#include "gaMessage.h"
#include "gaCollisionPoint.h"
#include "gaComponent.h"
#include "Collider.h"

#include "../flightRecorder/Entity.h"

class fwCylinder;
class fwScene;
class fwMesh;

class dfSuperSector;
struct recordEntity;

namespace GameEngine {
	enum ClassID {
		Entity = Framework::ClassID::Object3D | (1 << 1),
		Sector = Entity | (1 << 5),
		Actor = Entity | (1 << 6),
		GUI = Entity | (1 << 7)
	};
};

using namespace GameEngine;

class gaEntity : public fwObject3D
{
protected:
	uint32_t m_entityID = 0;

	bool m_physical = false;						// will interact physicaly with other object (like block move or be blocked)
	bool m_gravity = true;							// does gravity effect the entity
	bool m_collideSectors = true;					// entity has no intercation, will not be tested during collision test
	bool m_canStep = false;							// can the entity step up or down a stair
	bool m_hasCollider = false;						// entity needs to be tested with collider
	bool m_movable = true;							// entity can be pushed
	bool m_processMessages = true;					// shall the entity receive messages for processing
	bool m_falling = false;							// the physic engine is falling the entity
#ifdef _DEBUG
	bool m_displayAABB=false;						// display collision box
	bool m_debugPhysics = false;					// physic engine logs
#endif
	uint32_t m_timer = 0;							// number of components waiting for a gaMessage::TIMER every frame

	GameEngine::Transform m_transforms;				// transforms to move the object

	fwAABBox m_modelAABB;							// model space AABB
	fwAABBox m_worldBounding;						// AABB bounding box in world opengl space
	time_t m_animation_time = 0;					// elapsed time when running animation

	float m_step = 0;								// how up/down can the actor step over

	fwScene* m_scene = nullptr;						// if the entity has a mesh added to a scene
	fwMesh* m_mesh = nullptr;

	std::vector<std::tuple<gaComponent*, uint32_t>> m_components;			// all components of the entity
	std::map<std::string, void*> m_attributes;		// dictionary of attributes

	dfSuperSector* m_supersector = nullptr;			// cached super_sector hosting the object
	Collider m_collider;							// if there is a collider
	float m_radius = -1;							// radius of the collision box (can be different from the AABB)
	float m_height = -1;							// height of the collision box (can be different from the AABB)

public:
	enum Flag {
		DONT_DELETE,
		DELETE_AT_EXIT,
		EXTEND_AABB,
		DONT_EXTEND_AABB
	};
	gaEntity(uint32_t mclass);
	gaEntity(uint32_t mclass, const std::string& name);
	gaEntity(uint32_t mclass, const std::string& name, const glm::vec3& position);
	gaEntity(flightRecorder::Entity* record);
	gaEntity(gaEntity*);

	// external creator
	static void *create(void* record) {
		return new gaEntity((flightRecorder::Entity*)record);
	};

	// manage components
	void addComponent(gaComponent* component, uint32_t flag = Flag::DONT_DELETE);			// extend the components of the entity
	gaComponent* findComponent(int type);				// check all components to find one with the proper type
	inline uint32_t components(void) { return m_components.size(); };

	// getter/setter
	inline int entityID(void) { return m_entityID; };
	inline const std::string& name(void) { return m_name; };
	inline void name(const std::string& name) { m_name = name; };
	inline bool physical(void) { return m_physical; };
	inline void physical(bool p) { m_physical = p; };
	inline bool gravity(void) { return m_gravity; };
	inline void gravity(bool p) { m_gravity = p; };
	inline void falling(bool p) { m_falling= p; };
	inline bool falling(void) { return m_falling; };
	inline bool canStep(void) { return m_canStep; };
	inline bool collideSectors(void) { return m_collideSectors; };
	inline void collideSectors(bool c) { m_collideSectors = c; };
	void hasCollider(bool p);								 // Activate the collider(based on the worldAABB)
	inline bool hasCollider(void) { return m_hasCollider; };
	inline bool movable(void) { return m_movable; };
	inline void movable(bool b) { m_movable = b; };
	inline fwAABBox& worldAABB(void) { return m_worldBounding; };
	inline void worldAABB(const glm::vec3 p1, glm::vec3 p2) { m_worldBounding.set(p1, p2); };
	inline const fwAABBox& modelAABB(void) { return m_modelAABB; };
	inline GameEngine::Transform& transform(void) { return m_transforms; };
	inline GameEngine::Transform* pTransform(void) { return &m_transforms; };
	inline void superSector(dfSuperSector* s) { m_supersector = s; };
	inline float step(void) { return m_step; };
	void timer(bool onOff);												// manage number of components needing timer events
	inline bool timer(void) { return m_timer > 0; };
	inline void discardMessages(void) { m_processMessages = false; };	// discard any incoming message
	inline bool processMessages(void) { return m_processMessages; };	// shall we process incoming message
#ifdef _DEBUG
	inline void debugAABB(bool b) { m_displayAABB = b; };				// display or not the collision box of the object
	inline bool debugPhysic(void) { return m_debugPhysics; };
	inline void debugPhysic(bool b) { m_debugPhysics = true; };
#endif

	void displayAABBox(void);							// display the world AABBox on screen

	void set(const std::string& v, void* ptr) { m_attributes[v] = ptr; };
	void* get(const std::string& v) { return m_attributes[v]; };

	void transform(GameEngine::Transform* transform);	// apply a transformation and update the worldAABB
	void undoTransform(void);							// cancel the last transformation

	float radius(void);									// maximum radius of the entity (radius of the AABB if not initialized)
	float height(void);									// maximum height of the entity (height of the AABB if not initialized)

	void addChild(gaEntity* entity, Flag flag=EXTEND_AABB);		// add an entity inside that one (and increase the AABB if needed)

	bool collideAABB(const fwAABBox& box);				// quick test to find AABB collision
	bool collideAABB(gaEntity const* with);				// quick test to find AABB collision
	virtual fwAABBox::Intersection intersect(
		const Collider& c, 
		glm::vec3& p);									// quick test to find AABB collision and return the collision point

	inline bool inAABBox(const glm::vec3& position) {
		return m_worldBounding.inside(position);
	};                                                  // test if vec3 inside the AABB

	virtual bool isPointInside(const glm::vec3& position);	// is the point inside the entity (free form)

	bool collide(gaEntity* entity, 
		const glm::vec3& forward, 
		const glm::vec3& down,
		std::vector<gaCollisionPoint>& collisions);		// extended collision using colliders
	bool collide(const GameEngine::Collider& collider,
		const glm::vec3& forward,
		const glm::vec3& down,
		std::vector<gaCollisionPoint>& collisions);		// extended collision using colliders
	bool warpThrough(const GameEngine::Collider& collider,
		const glm::vec3& old_position,
		std::vector<gaCollisionPoint>& collisions);		// check if the entity moved so fast it went trough another one
	void modelAABB(const fwAABBox& box);				// set the model space AABB

	gaMessage* sendMessage(const std::string& target,
		int action,
		int value = 0,
		void* extra = nullptr);							// send a message to an other entity
	gaMessage* sendMessage(const std::string& target, int action, uint32_t value, const glm::vec3& v3value, void* extra = nullptr);
	gaMessage* sendMessage(const std::string& target, int action, const glm::vec3& value, void* extra = nullptr);
	gaMessage* sendMessage(const std::string& target, int action, uint32_t value, float fvalue, const glm::vec3& v3value, void* extra = nullptr);
	gaMessage* sendMessage(int action, int value = 0, void* extra = nullptr);
	gaMessage* sendMessage(int action, int value, float fvalue, void* extra = nullptr);
	gaMessage* sendMessage(int action, int value, const glm::vec3& v3alue, void* extra = nullptr);
	gaMessage* sendMessage(int action, int value, float fvalue, const glm::vec3& v3alue, void* extra = nullptr);
	gaMessage* sendMessage(int action, const glm::vec3& v3alue, void* extra = nullptr);

	gaMessage* sendMessageToWorld(int action,
		int value = 0,
		void* extra = nullptr);							// send a message to the world
	gaMessage* sendDelayedMessageToWorld(int action,
		int value = 0,
		void* extra = nullptr);							// send a message to the world
	void sendInternalMessage(int action,
		int value = 0,
		void* extra = nullptr);							// send internal message to all components of the current entity
	void sendInternalMessage(int action,
		const glm::vec3& value);						// send internal message to all components of the current entity
	gaMessage* sendDelayedMessage(int action,
		int value = 0,
		float fvalue = 0.0f,
		void* extra = nullptr);							// send a delayed message to myself

	virtual void add2scene(fwScene* scene);				// if the entity has a mesh, add to the scene
	virtual void collideWith(gaEntity*) {};				// inform another entity of a collision
	virtual void updateWorldAABB(void);					// update the world AABB based on position
	virtual void moveTo(const glm::vec3& position);		// move the the object and update the AABB
	virtual bool update(time_t t) {	return false;};		// update based on timer
	virtual void dispatchMessage(gaMessage* message);	// let an entity deal with a situation
	virtual bool checkCollision(fwCylinder& bounding, 
		glm::vec3& direction, 
		glm::vec3& intersection, 
		std::list<gaCollisionPoint>& collisions);		// extended collision test after a sucessfull AABB collision
	virtual dfSuperSector* superSector(void) {			// return the supersector the entity is in
		return m_supersector; 
	};

	// flight recorder
	virtual int recordSize(void) {
		return sizeof(flightRecorder::Entity);
	}													// size of one record
	uint32_t componentsSize(void);						// size of all the components
	uint32_t recordComponents(void* p);					// save the components starting at p
	void loadComponents(void* p);						// load the components starting at p
	virtual uint32_t recordState(void* record);			// return a record of the entity state (for debug)
	virtual void loadState(void* record);// reload an entity state from a record

#ifdef _DEBUG
	// debugger
	virtual void debugGUI(const std::string& display, bool* close);				// debug the entity
#endif

	~gaEntity();
};
