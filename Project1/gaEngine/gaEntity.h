#pragma once

#include <string>
#include <list>
#include <glm/vec3.hpp>

#include "../framework/fwAABBox.h"
#include "gaMessage.h"

class gaEntity
{
protected:
	std::string m_name;
	int m_entityID = 0;
	int m_class = 0;
	glm::vec3 m_position = glm::vec3(0);	// position in gl space
	glm::vec3 m_rotation = glm::vec3(0);	
	fwAABBox m_modelAABB;					// model space AABB
	fwAABBox m_worldBounding;				// AABB bounding box in world gl space
	std::list<gaEntity*> m_children;		// included entities

public:
	gaEntity(int mclass, const std::string& name);
	gaEntity(int mclass, const std::string& name, const glm::vec3& position);

	const std::string& name(void) { return m_name; };
	bool is(int mclass) { return m_class == m_class; };

	void addChild(gaEntity* entity);					// add an entity inside that one (and increase the AABB if needed)
	bool collideAABB(fwAABBox& box);					// quick test to find AABB collision
	void modelAABB(const fwAABBox& box);				// set the model space AABB
	void drawBoundingBox(void);							// create a world boundingbox mesh
	void rotate(const glm::vec3& rotation);				// rotate the object and update the AABB

	virtual void collideWith(gaEntity*) {};				// inform another entity of a collision
	virtual void updateWorldAABB(void);					// update the world AABB based on position
	virtual void moveTo(const glm::vec3& position);		// move the the object and update the AABB
	virtual bool update(time_t t) {	return false;};		// update based on timer
	virtual void dispatchMessage(gaMessage* message) {};// let an entity deal with a situation

	~gaEntity();
};