#pragma once

#include <string>
#include <list>
#include <glm/vec3.hpp>

#include "../framework/fwAABBox.h"
#include "../darkforces/dfMessage.h"

class gaEntity
{
protected:
	std::string m_name;
	int m_entityID = 0;
	int m_class = 0;
	glm::vec3 m_position = glm::vec3(0);	// position in gl space
	fwAABBox m_modelAABB;					// model space AABB
	fwAABBox m_worldBounding;				// AABB bounding box in world gl space
public:
	gaEntity(int mclass, const std::string& name);
	gaEntity(int mclass, const std::string& name, const glm::vec3& position);

	const std::string& name(void) { return m_name; };
	bool is(int mclass) { return m_class == m_class; };

	bool collideAABB(fwAABBox& box);					// quick test to find AABB collision
	void modelAABB(const fwAABBox& box);				// set the model space AABB
	void drawBoundingBox(void);							// create a world boundingbox mesh

	virtual void collideWith(gaEntity*) {};				// inform another entity of a collision
	virtual void updateWorldAABB(void);					// update the world AABB based on position
	virtual void moveTo(const glm::vec3& position);		// update the object position
	virtual bool update(time_t t) {	return false;};		// update based on timer
	virtual void dispatchMessage(dfMessage* message) {};// let an entity deal with a situation

	~gaEntity();
};