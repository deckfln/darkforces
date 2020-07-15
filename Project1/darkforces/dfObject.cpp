#include "dfObject.h"

#include "../config.h"
#include "../framework/math/fwCylinder.h"

#include "../gaEngine/gaCollisionPoint.h"
#include "../gaEngine/gaDebug.h"

#include "dfModel.h"
#include "dfSprites.h"
#include "dfLevel.h"
#include "dfCollision.h"
#include "dfComponent/dfComponentActor.h"

static int g_ids = 0;

const int DF_SHIELD_ENERGY = 100;
const int DF_ENERGY_ENERGY = 100;

/**
 *
 */
dfObject::dfObject(dfModel *source, glm::vec3& position, float ambient, int type):
	gaEntity(DF_ENTITY_OBJECT, source->name() + "(" + std::to_string(g_ids) + ")"),
	m_source(source),
	m_position_lvl(position),
	m_ambient(ambient),
	m_is(type),
	m_objectID(g_ids++)
{
	modelAABB(m_source->bounding());
	moveTo(position);
}

/**
 * Check the name of th associated WAX
 */
bool dfObject::named(std::string name)
{
	return m_source->named(name);
}

/**
 * Stack up logics
 */
void dfObject::logic(int logic)
{
	m_logics |= logic;

	// Only enemies cannot be walked through
	if (m_logics & DF_ENEMIES) {
		physical(true);
	}

	// start the animation loop
	if (m_logics & DF_LOGIC_ANIM) {
		g_gaWorld.sendMessageDelayed(m_name, m_name, GA_MSG_TIMER, 0, nullptr);
	}
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
const std::string& dfObject::model(void)
{
	return m_source->name();
}

/**
 * Update the object position (given in level space) and update the worldboundingBox(in gl space)
 */
void dfObject::moveTo(const glm::vec3& position)
{
	m_position_lvl = position;
	dfLevel::level2gl(m_position_lvl, m_position);

	m_dirtyPosition = true;

	gaEntity::moveTo(m_position);
}

/**
 * Animate the object frame
 */
bool dfObject::update(time_t t)
{
	return false;
}

/**
 *  reaction on a collision with another entity
 */
void dfObject::collideWith(gaEntity* entity)
{
	// extra security
	if (entity == nullptr) {
		return;
	}

	if (m_logics & DF_LOGIC_ITEM_SHIELD) {
		// ADD ARMOR
		if (entity->findComponent(DF_COMPONENT_ACTOR)) {
			// if the collider is a DF_ACTOR
			// send shield from me to the actor
			gaMessage* msg = g_gaWorld.sendMessage(
				m_name,
				entity->name(),
				DF_MESSAGE_ADD_SHIELD,
				DF_SHIELD_ENERGY,
				nullptr);

			// and remove the object from the scene
			moveTo(glm::vec3(0));
		}
	}
	else if (m_logics & DF_LOGIC_ITEM_ENERGY) {
		// ADD ENERGY
		if (entity->findComponent(DF_COMPONENT_ACTOR)) {
			// if the collider is a DF_ACTOR
			// send energy from me to the actor
			gaMessage* msg = g_gaWorld.sendMessage(
				m_name,
				entity->name(),
				DF_MESSAGE_ADD_ENERGY,
				DF_ENERGY_ENERGY,
				nullptr);

			// and remove the object from the scene
			m_position = glm::vec3(0);
			moveTo(glm::vec3(0));
		}
	}
}

/**
 * update the world AABB based on position
 */
void dfObject::updateWorldAABB(void)
{
	// extract the radius from the AABB
	m_radius = std::max(abs(m_worldBounding.m_p1.x - m_worldBounding.m_p.x), abs(m_worldBounding.m_p1.z - m_worldBounding.m_p.z)) / 2.0f;

	gaEntity::updateWorldAABB();
}

/**
 * extended collision test after a sucessfull AABB collision
 * consider the object is a cylinder
 */
bool dfObject::checkCollision(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::list<gaCollisionPoint>& collisions)
{
	// for object with radius 0, ignore any collision
	if (m_radius == 0) {
		return false;
	}

	// build the target cylinder
	// and convert to AABB for fast test
	fwCylinder cyl(bounding, direction);

	fwAABBox aabb(cyl);	
	glm::vec3 playerCenter = aabb.center();

	// inside the AABB, now check if the 2 cylinders checkCollision
	intersection = cyl.position();
	glm::vec3 c = m_worldBounding.center();
	float x1 = c.x - intersection.x;
	float y1 = c.z - intersection.z;
	float distance = sqrt(x1*x1 + y1*y1);
	float mxDistance = m_radius + bounding.radius();
	if (distance > mxDistance) {
		return false;
	}

	// send a message: YES we checkCollision
	collisions.push_back(gaCollisionPoint(fwCollisionLocation::COLLIDE, intersection, this));

	// ALL these objects can be traversed
	if (isLogic(DF_LOGIC_ITEM_SHIELD | DF_LOGIC_ITEM_ENERGY | DF_LOGIC_LIFE | DF_LOGIC_REVIVE)) {
		return false;
	}

	std::string message;
	int local_collision = 0;

	// test bottom and top IF the player is on top
	if (aabb.verticalAlign(playerCenter)) {
		if (aabb.m_p.y < m_worldBounding.m_p1.y && aabb.m_p1.y > m_worldBounding.m_p1.y) {
			intersection.y = m_worldBounding.m_p1.y;
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::BOTTOM, intersection, this));

#ifdef DEBUG
			std::string message = " BOTTOM z=" + std::to_string(intersection.y);
			gaDebugLog(FULL_DEBUG, "dfObject::checkCollision", message);
#endif
		}
		if (aabb.m_p1.y > m_worldBounding.m_p.y && aabb.m_p1.y < m_worldBounding.m_p1.y) {
			intersection.y = m_worldBounding.m_p.y;
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::TOP, intersection, this));
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

		collisions.push_back(gaCollisionPoint(c, intersection, this));

#ifdef DEBUG
		std::string message = " " + p + " z=" + std::to_string(intersection.y);
		gaDebugLog(LOW_DEBUG, "dfObject::checkCollision", message);
#endif
	}
	return true;
}

dfObject::~dfObject()
{
}
