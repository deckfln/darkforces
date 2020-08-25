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

class fwCylinder;
class fwScene;
class fwMesh;

class dfSuperSector;

using namespace GameEngine;

class gaEntity: public fwObject3D
{
protected:
	std::string m_name;
	int m_entityID = 0;
	int m_class = 0;

	glm::mat3x3 m_physic = glm::mat3x3(0);
	time_t m_physic_time_elpased = 0;				// physic engine

	fwAABBox m_modelAABB;							// model space AABB
	fwAABBox m_worldBounding;						// AABB bounding box in world opengl space
	bool m_physical = false;						// if this entity has a body to checkCollision with
	time_t m_animation_time = 0;					// elapsed time when running animation
	std::list<gaEntity*> m_children;				// included entities

	fwScene* m_scene = nullptr;						// if the entity has a mesh added to a scene
	fwMesh* m_mesh = nullptr;

	std::vector<gaComponent*> m_components;			// all components of the entity
	std::map<std::string, void*> m_attributes;		// dictionary of attributes

	dfSuperSector* m_supersector = nullptr;			// cached super_sector hosting the object
	Collider m_collider;							// if there is a collider

public:
	gaEntity(int mclass, const std::string& name);
	gaEntity(int mclass, const std::string& name, const glm::vec3& position);

	int entityID(void) { return m_entityID; };
	void addComponent(gaComponent* component);			// extend the components of the entity

	const std::string& name(void) { return m_name; };
	bool is(int mclass) { return m_class == m_class; };
	void physical(bool p) { m_physical = p; };
	bool physical(void) { return m_physical; };
	const fwAABBox& worldAABB(void) { return m_worldBounding; };
	const fwAABBox& modelAABB(void) { return m_modelAABB; };
	void superSector(dfSuperSector* s) { m_supersector = s; };

	void set(const std::string& v, void* ptr) { m_attributes[v] = ptr; };
	void* get(const std::string& v) { return m_attributes[v]; };

	void transform(GameEngine::Transform* transform);	// apply a transformation and update the worldAABB

	gaComponent *findComponent(int type);				// check all components to find one with the proper type
	void addChild(gaEntity* entity);					// add an entity inside that one (and increase the AABB if needed)
	bool collideAABB(const fwAABBox& box);				// quick test to find AABB collision
	bool collide(gaEntity* entity, 
		const glm::vec3& forward, 
		const glm::vec3& down,
		std::list<gaCollisionPoint>& collisions);		// extended collision using colliders
	bool collide(GameEngine::Collider collider,
		const glm::vec3& forward,
		const glm::vec3& down,
		std::list<gaCollisionPoint>& collisions);		// extended collision using colliders
	bool warpThrough(GameEngine::Collider collider,
		const glm::vec3& old_position,
		glm::vec3& collision);							// check if the entity moved so fast it went trough another one
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

	void engagePhysics(const glm::vec3& pos, 
		const glm::vec3& direction);					// engage the physic engine
	void applyPhysics(time_t delta, 
		GameEngine::Transform* transform);				// execute the physic engine


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

	virtual dfSuperSector* superSector(void) { return m_supersector; };

	~gaEntity();
};