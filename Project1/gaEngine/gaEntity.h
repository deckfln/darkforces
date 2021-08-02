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

using namespace GameEngine;

class gaEntity: public fwObject3D
{
protected:
	std::string m_name;
	int m_entityID = 0;
	int m_class = 0;

	bool m_physical = false;						// if this entity has a body to checkCollision with
	bool m_gravity = true;							// does gravity effect the entity
	bool m_collideSectors = true;					// does the entity collide with sectors
	bool m_canStep = false;							// can the entity step up or down a stair
	bool m_hasCollider = false;						// entity needs to be tested with collider

	GameEngine::Transform m_transforms;				// transforms to move the object

	fwAABBox m_modelAABB;							// model space AABB
	fwAABBox m_worldBounding;						// AABB bounding box in world opengl space
	time_t m_animation_time = 0;					// elapsed time when running animation
	std::list<gaEntity*> m_children;				// included entities

	fwScene* m_scene = nullptr;						// if the entity has a mesh added to a scene
	fwMesh* m_mesh = nullptr;

	std::vector<gaComponent*> m_components;			// all components of the entity
	std::map<std::string, void*> m_attributes;		// dictionary of attributes

	dfSuperSector* m_supersector = nullptr;			// cached super_sector hosting the object
	Collider m_collider;							// if there is a collider

	int m_defaultCollision = gaMessage::Flag::COLLIDE_ENTITY;
	std::map<std::string, gaEntity*> m_sittingOnTop;// cached list of the entities sitting on top of that one

public:
	gaEntity(int mclass, const std::string& name);
	gaEntity(int mclass, const std::string& name, const glm::vec3& position);
	gaEntity(flightRecorder::Entity* record);

	inline int entityID(void) { return m_entityID; };
	void addComponent(gaComponent* component);			// extend the components of the entity

	inline bool is(int mclass) { return m_class == mclass; };

	inline const std::string& name(void) { return m_name; };
	inline bool physical(void) { return m_physical; };
	inline void physical(bool p) { m_physical = p; };
	inline bool gravity(void) { return m_gravity; };
	inline void gravity(bool p) { m_gravity = p; };
	inline bool canStep(void) { return m_canStep; };
	inline bool collideSectors(void) { return m_collideSectors; };
	inline void hasCollider(bool p) { m_hasCollider = p; };
	inline bool hasCollider(void) { return m_hasCollider; };
	inline const fwAABBox& worldAABB(void) { return m_worldBounding; };
	inline void worldAABB(const glm::vec3 p1, glm::vec3 p2) { m_worldBounding.set(p1, p2); };
	inline const fwAABBox& modelAABB(void) { return m_modelAABB; };
	inline std::map<std::string, gaEntity*>& sittingOnTop(void) { return m_sittingOnTop; };
	inline GameEngine::Transform& transform(void) { return m_transforms; };
	inline GameEngine::Transform* pTransform(void) { return &m_transforms; };
	inline int defaultCollision(void) { return m_defaultCollision; };

	inline void superSector(dfSuperSector* s) { m_supersector = s; };

	void set(const std::string& v, void* ptr) { m_attributes[v] = ptr; };
	void* get(const std::string& v) { return m_attributes[v]; };

	void transform(GameEngine::Transform* transform);	// apply a transformation and update the worldAABB

	float radius(void);									// maximum radius of the entity

	gaComponent *findComponent(int type);				// check all components to find one with the proper type
	void addChild(gaEntity* entity);					// add an entity inside that one (and increase the AABB if needed)

	bool collideAABB(const fwAABBox& box);				// quick test to find AABB collision
	bool collideAABB(gaEntity const* with);				// quick test to find AABB collision

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

	void sendMessage(const std::string& target,
		int action,
		int value = 0,
		void* extra = nullptr);							// send a message to an other entity
	void sendMessageToWorld(int action,
		int value = 0,
		void* extra = nullptr);							// send a message to the world
	void sendDelayedMessageToWorld(int action,
		int value = 0,
		void* extra = nullptr);							// send a message to the world
	void sendInternalMessage(int action,
		int value = 0,
		void* extra = nullptr);							// send internal message to all components of the current entity
	void sendDelayedMessage(int action,
		int value = 0,
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
	virtual void OnWorldInsert(void) {};				// trigger when inserted in a gaWorld
	virtual void OnWorldRemove(void) {};				// trigger when from the gaWorld
	virtual dfSuperSector* superSector(void) {			// return the supersector the entity is in
		return m_supersector; 
	};
	virtual int recordSize(void) {
		return sizeof(flightRecorder::Entity);
	}													// size of one record
	virtual void recordState(void* record);				// return a record of the entity state (for debug)
	virtual void loadState(flightRecorder::Entity* record);// reload an entity state from a record
	virtual void debugGUI(bool *close);					// debug the entity

	~gaEntity();
};
