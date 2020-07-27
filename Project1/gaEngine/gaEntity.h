#pragma once

#include <string>
#include <list>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../framework/fwAABBox.h"
#include "gaMessage.h"
#include "gaCollisionPoint.h"
#include "gaComponent.h"

class fwCylinder;
class fwScene;
class fwMesh;

class gaEntity
{
protected:
	std::string m_name;
	int m_entityID = 0;
	int m_class = 0;
	glm::vec3 m_position = glm::vec3(0);	// position in gl space
	glm::vec3 m_futurePosition=glm::vec3(0);// want to move to position
	glm::vec3 m_rotation = glm::vec3(0);
	glm::quat m_quaternion = glm::quat(0,0,0,0);
	fwAABBox m_modelAABB;					// model space AABB
	fwAABBox m_worldBounding;				// AABB bounding box in world gl space
	bool m_physical = false;				// if this entity has a body to checkCollision with
	time_t m_time=0;						// elapsed time when running animation
	std::list<gaEntity*> m_children;		// included entities

	fwScene* m_scene = nullptr;				// if the entity has a mesh added to a scene
	fwMesh* m_mesh = nullptr;

	std::vector<gaComponent*> m_components;	// all components of the entity

public:
	gaEntity(int mclass, const std::string& name);
	gaEntity(int mclass, const std::string& name, const glm::vec3& position);

	int entityID(void) { return m_entityID; };
	void addComponent(gaComponent* component);			// extend teh components of the entity

	const std::string& name(void) { return m_name; };
	const glm::vec3& position(void) { return m_position; };
	bool is(int mclass) { return m_class == m_class; };
	void physical(bool p) { m_physical = p; };
	bool physical(void) { return m_physical; };
	const fwAABBox& worldAABB(void) { return m_worldBounding; };

	gaComponent *findComponent(int type);				// check all components to find one with the proper type
	void addChild(gaEntity* entity);					// add an entity inside that one (and increase the AABB if needed)
	bool collideAABB(fwAABBox& box);					// quick test to find AABB collision
	void modelAABB(const fwAABBox& box);				// set the model space AABB
	void drawBoundingBox(void);							// create a world boundingbox mesh
	void rotate(const glm::vec3& rotation);				// rotate the object and update the AABB
	float distanceTo(gaEntity* other);					// distance between the 2 entities
	float distanceTo(const glm::vec3& p);				// distance from the entity position to the point

	void sendMessage(const std::string& target,
		int action,
		int value = 0,
		void* extra = nullptr);							// send a message to an other entity
	void sendMessageToWorld(int action,
		int value = 0,
		void* extra = nullptr);							// send a message to the world
	void sendInternalMessage(int action,
		int value = 0,
		void* extra = nullptr);							// send internal message to all components of the current entity

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

	~gaEntity();
};