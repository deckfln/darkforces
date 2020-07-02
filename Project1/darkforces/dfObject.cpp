#include "dfObject.h"

#include "../config.h"
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
	updateWorldAABB();

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
 * Update the world AABB from a simple source
 */
void dfObject::updateWorldAABB(void)
{
	updateWorldAABB(m_source->bounding());
}

void dfObject::updateWorldAABB(const fwAABBox& box)
{
	m_worldBounding.translateFrom(box, m_position_gl);

	// extract the radius from the AABB
	m_radius = std::max(abs(m_worldBounding.m_p1.x - m_worldBounding.m_p.x), abs(m_worldBounding.m_p1.z - m_worldBounding.m_p.z))/2.0f;
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
	glm::vec3 playerCenter = aabb.center();

	if (m_worldBounding.intersect(aabb)) {
		// for object with radius 0, just send the message, we don't want to trigger a physical collision
		if (m_radius == 0) {
			collisions.push_back(fwCollisionPoint(fwCollisionLocation::COLLIDE, intersection));
			return true;
		}

		// inside the AABB, now check if the 2 cylinders collide
		intersection = cyl.position();
		glm::vec3 c = m_worldBounding.center();
		float x1 = c.x - intersection.x;
		float y1 = c.z - intersection.z;
		float sqDistance = sqrt(x1*x1 + y1*y1);
		float mxsqDistance = m_radius + bounding.radius();
		if (sqDistance > mxsqDistance) {
			return false;
		}

		// ALWAYS send a message: YES we collide
		collisions.push_back(fwCollisionPoint(fwCollisionLocation::COLLIDE, intersection));

		std::string message;
		int local_collision = 0;

		// test bottom and top IF the player is on top
		if (aabb.verticalAlign(playerCenter)) {
			if (aabb.m_p.y < m_worldBounding.m_p1.y && aabb.m_p1.y > m_worldBounding.m_p1.y) {
				intersection.y = m_worldBounding.m_p1.y;
				collisions.push_back(fwCollisionPoint(fwCollisionLocation::BOTTOM, intersection));

#ifdef DEBUG
				std::string message = " BOTTOM z=" + std::to_string(intersection.y);
				gaDebugLog(FULL_DEBUG, "dfObject::checkCollision", message);
#endif
			}
			if (aabb.m_p1.y > m_worldBounding.m_p.y && aabb.m_p1.y < m_worldBounding.m_p1.y) {
				intersection.y = m_worldBounding.m_p.y;
				collisions.push_back(fwCollisionPoint(fwCollisionLocation::TOP, intersection));
#ifdef DEBUG
				std::string message = " TOP z=" + std::to_string(intersection.y);
				gaDebugLog(LOW_DEBUG, "dfObject::checkCollision", message);
#endif
			}
		}

		// test front,back and left (only if moving around)
		if (direction.x != 0 && direction.z != 0) {
			glm::vec3 player2object = aabb.to(m_worldBounding);
			player2object = glm::normalize(player2object);
			glm::vec3 d = glm::normalize(direction);

			// test if bottom or top part
			glm::vec3 center = aabb.center();
			if (m_worldBounding.m_p1.y < center.y) {
				local_collision = 1;
			}
			if (m_worldBounding.m_p.y > center.y) {
				local_collision += 2;
			}

			float delta = glm::dot(d, player2object);
			fwCollisionLocation c;
			std::string p;
			if (delta > 0.01) {
				switch (local_collision) {
				case 0:	// only front collision
				case 3: // both top & bottom => full front
					c = fwCollisionLocation::FRONT;
					p = "FRONT";
					break;
				case 1:
					c = fwCollisionLocation::FRONT_BOTTOM;
					intersection.y = m_worldBounding.m_p1.y;
					p = "FRONT_BOTTOM";
					break;
				case 2:
					c = fwCollisionLocation::FRONT_TOP;
					intersection.y = m_worldBounding.m_p.y;
					p = "FRONT_TOP";
					break;
				}
			}
			else if (delta >= -0.01) {
				c = fwCollisionLocation::LEFT;
				p = "LEFT";
			}
			else {
				c = fwCollisionLocation::BACK;
				p = "BACK";
			}

			collisions.push_back(fwCollisionPoint(c, intersection));

#ifdef DEBUG
			std::string message = " " + p + " z=" + std::to_string(intersection.y);
			gaDebugLog(LOW_DEBUG, "dfObject::checkCollision", message);
#endif
		}
		return true;
	}

	return false;
}

dfObject::~dfObject()
{
}
