#include "dfObject.h"

#include <imgui.h>

#include "../config.h"
#include "../framework/math/fwCylinder.h"

#include "../gaEngine/gaCollisionPoint.h"
#include "../gaEngine/gaDebug.h"

#include "dfModel.h"
#include "dfSprites.h"
#include "dfLevel.h"
#include "dfCollision.h"
#include "dfComponent/dfComponentActor.h"
#include "dfComponent/dfComponentLogic.h"
#include "dfObject/dfSprite.h"

static int g_ids = 0;

/**
 *
 */
dfObject::dfObject(dfModel *source, const glm::vec3& position, float ambient, int type, uint32_t objectID):
	gaEntity(DF_ENTITY_OBJECT, source->name() + "(" + std::to_string(objectID) + ")"),
	m_source(source),
	m_position_lvl(position),
	m_ambient(ambient),
	m_is(type),
	m_objectID(g_ids++)
{
	modelAABB(m_source->modelAABB());
	moveTo(position);
}

/**
 * create a full object from a saved state
 */
dfObject::dfObject(flightRecorder::DarkForces::dfObject* record) :
	gaEntity(&record->entity)
{
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
void dfObject::logic(uint32_t logic)
{
	m_logics |= logic;

	// Only enemies cannot be walked through
	if (m_logics & DF_LOGIC_PHYSICAL) {
		physical(true);
	}

	// start the animation loop
	dfComponentLogic* lc = (dfComponentLogic*)findComponent(DF_COMPONENT_LOGIC);
	if (lc) {
		lc->logic(logic);
	}

	if (m_logics & dfLogic::ANIM) {
		g_gaWorld.sendMessageDelayed(m_name, m_name, gaMessage::TIMER, 0, nullptr);
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
bool dfObject::isLogic(uint32_t logic)
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
void dfObject::moveTo(const glm::vec3& pposition)
{
	m_position_lvl = pposition;

	glm::vec3 p;
	dfLevel::level2gl(m_position_lvl, p);
	m_dirtyPosition = true;
	gaEntity::moveTo(p);
}

/**
 * Animate the object frame
 */
bool dfObject::update(time_t t)
{
	return false;
}

/**
 * object to drop in the scene at the current position
 */
void dfObject::drop(uint32_t logic)
{
	dfSprite* obj=nullptr;

	// constructor of a sprite expects a level space
	glm::vec3 p;
	dfLevel::gl2level(position(), p);
	switch (logic) {
	case dfLogic::DEAD_MOUSE:
		obj = new dfSprite("DEDMOUSE.FME", p, 1.0f, OBJECT_FME);
		obj->hasCollider(false);
		break;
	case dfLogic::ITEM_BATTERY:
		obj = new dfSprite("IBATTERY.FME", p, 1.0f, OBJECT_FME);
		obj->hasCollider(true);
		break;
	case dfLogic::ITEM_RIFLE:
		obj = new dfSprite("IST-GUNI.FME", p, 1.0f, OBJECT_FME);
		obj->hasCollider(true);
		break;
	case dfLogic::ITEM_POWER:
		obj = new dfSprite("IPOWER.FME", p, 1.0f, OBJECT_FME);
		obj->hasCollider(true);
		break;
	case dfLogic::ITEM_ENERGY:
		obj = new dfSprite("IENERGY.FME", p, 1.0f, OBJECT_FME);
		obj->hasCollider(true);
		break;
	case dfLogic::RED_KEY:
		obj = new dfSprite("IKEYR.FME", p, 1.0f, OBJECT_FME);
		obj->hasCollider(true);
		break;
	}
	obj->logic(logic);
	obj->physical(false);	// objects can be traversed and are not subject to gravity
	obj->gravity(false);

	g_gaWorld.addClient(obj);
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
 * populate the super sector if it is not there
 */
dfSuperSector* dfObject::superSector(void)
{
	if (m_supersector == nullptr) {
		m_supersector = m_sector->parent();
	}

	return m_supersector;
}

/**
 * extended collision test after a successful AABB collision
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
	collisions.push_back(gaCollisionPoint(fwCollisionLocation::COLLIDE, intersection, nullptr));

	// ALL these objects can be traversed
	if (isLogic(dfLogic::ITEM_SHIELD | dfLogic::ITEM_ENERGY | dfLogic::LIFE | dfLogic::REVIVE)) {
		return false;
	}

	std::string message;
	int local_collision = 0;

	// test bottom and top IF the player is on top
	if (aabb.verticalAlign(playerCenter)) {
		if (aabb.m_p.y < m_worldBounding.m_p1.y && aabb.m_p1.y > m_worldBounding.m_p1.y) {
			intersection.y = m_worldBounding.m_p1.y;
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::BOTTOM, intersection, nullptr));

#ifdef _DEBUG
			std::string message = " BOTTOM z=" + std::to_string(intersection.y);
			gaDebugLog(FULL_DEBUG, "dfObject::checkCollision", message);
#endif
		}
		if (aabb.m_p1.y > m_worldBounding.m_p.y && aabb.m_p1.y < m_worldBounding.m_p1.y) {
			intersection.y = m_worldBounding.m_p.y;
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::TOP, intersection, nullptr));
#ifdef _DEBUG
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

		collisions.push_back(gaCollisionPoint(c, intersection, nullptr));

#ifdef _DEBUG
		std::string message = " " + p + " z=" + std::to_string(intersection.y);
		gaDebugLog(LOW_DEBUG, "dfObject::checkCollision", message);
#endif
	}
	return true;
}

/**
 * return a record of the entity state (for debug)
 */
void* frCreate_dfObject(void* record) {
	return new dfObject((flightRecorder::DarkForces::dfObject*)record);
}

/**
 * return a record of the entity state (for debug)
 */
void dfObject::recordState(void* r)
{
	gaEntity::recordState(r);
	flightRecorder::DarkForces::dfObject* record = (flightRecorder::DarkForces::dfObject*)r;

	record->entity.classID = flightRecorder::TYPE::DF_ENTITY_OBJECT;
	record->entity.size = sizeof(flightRecorder::DarkForces::dfObject);

	record->position_level = m_position_lvl;
}

/**
 * reload an entity state from a record
 */
void dfObject::loadState(flightRecorder::Entity* r)
{
	gaEntity::loadState(r);
	flightRecorder::DarkForces::dfObject* record = (flightRecorder::DarkForces::dfObject*)r;
	m_position_lvl = record->position_level;
}

/**
 * Add dedicated component debug the entity
 */
void dfObject::debugGUIChildClass(void)
{
	ImGui::Text("LevelPos %.2f %.2f %.2f", m_position_lvl.x, m_position_lvl.y, m_position_lvl.z);
}

dfObject::~dfObject()
{
}
