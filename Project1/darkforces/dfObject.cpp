#include "dfObject.h"

#include "../framework/math/fwCylinder.h"

#include "../gaEngine/gaBoundingBoxes.h"
#include "../gaEngine/gaDebug.h"

#include "dfModel.h"
#include "dfSprites.h"
#include "dfLevel.h"
#include "dfCollision.h"

static int g_ids = 0;

dfObject::dfObject(dfModel *source, glm::vec3& position, float ambient, int type):
	m_source(source),
	m_position_lvl(position),
	m_ambient(ambient),
	m_is(type),
	m_id(g_ids++)
{
	m_name = source->name() + "(" + std::to_string(m_id) + ")";
	update(position);
}

/**
 * Check the name of th associated WAX
 */
bool dfObject::named(std::string name)
{
	return m_source->named(name);
}

/**
 * Do we check collision ?
 */
bool dfObject::collision(void)
{
	return m_source->collision();
}

/**
 * test the nature of the object
 */
bool dfObject::is(int type)
{
	return m_is == type;
}

/**
 * test the logic
 */
bool dfObject::isLogic(int logic)
{
	return (m_logics & logic) != 0;
}

int dfObject::difficulty(void)
{
	return m_difficulty + 3;
}

/**
 * get the name of the model the object is based on
 */
std::string& dfObject::model(void)
{
	return m_source->name();
}

/**
 * Update the object position (given in level space) and update the worldboundingBox(in gl space)
 */
void dfObject::update(const glm::vec3& position)
{
	m_position_lvl = position;
	dfLevel::level2gl(m_position_lvl, m_position_gl);

	// take the opportunity to update the world bounding box
	m_worldBounding.translateFrom(m_source->bounding(), m_position_gl);

	if (m_meshAABB) {
		// and update the gl boundingbox
		m_meshAABB->translate(m_position_gl);
	}

	m_dirtyPosition = true;
}

/**
 * Animate the object frame
 */
bool dfObject::update(time_t t)
{
	return false;
}

/**
 * Stack up logics
 */
void dfObject::logic(int logic)
{
	m_logics |= logic;
}

/**
 * create a boundingbox mesh
 */
void dfObject::drawBoundingBox(void)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
}

/**
 * check if the object collide
 */
bool dfObject::checkCollision(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::list<fwCollisionPoint>& collisions)
{
	fwCylinder cyl(bounding, direction);

	fwAABBox aabb(cyl);	// convert to AABB for fast test

	if (m_worldBounding.intersect(aabb)) {
		fwCollisionLocation c;

		// test bottom and top
		if (aabb.m_p.y > m_worldBounding.m_p.y && aabb.m_p.y < m_worldBounding.m_p1.y) {
			intersection = cyl.position();
			intersection.y = m_worldBounding.m_p1.y;
			collisions.push_back(fwCollisionPoint(fwCollisionLocation::BOTTOM, intersection));
			c = fwCollisionLocation::BOTTOM;
			std::string message = " BOTTOM z=" + std::to_string(intersection.y);
			gaDebugLog(4, "dfObject::checkCollision", message);
			return true;
		}
		if (aabb.m_p.y < m_worldBounding.m_p.y && aabb.m_p1.y > m_worldBounding.m_p.y) {
			intersection = cyl.position();
			intersection.y = m_worldBounding.m_p.y;
			collisions.push_back(fwCollisionPoint(fwCollisionLocation::TOP, intersection));
			c = fwCollisionLocation::TOP;
			return true;
		}

		// test front,back and left
		glm::vec3 player2object = aabb.to(m_worldBounding);
		player2object = glm::normalize(player2object);

		float delta = glm::dot(direction, player2object);
		if (delta > 0.5) {
			collisions.push_back(fwCollisionPoint(fwCollisionLocation::FRONT, intersection));
			c = fwCollisionLocation::FRONT;
		}
		else if (delta >= -0.5) {
			collisions.push_back(fwCollisionPoint(fwCollisionLocation::LEFT, intersection));
			c = fwCollisionLocation::LEFT;
		}
		else {
			collisions.push_back(fwCollisionPoint(fwCollisionLocation::BACK, intersection));
			c = fwCollisionLocation::BACK;
		}

		return true;
	}

	return false;
}

dfObject::~dfObject()
{
}
