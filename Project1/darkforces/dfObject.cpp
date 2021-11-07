#include "dfObject.h"

#include <imgui.h>

#include "../config.h"
#include "../framework/math/fwCylinder.h"

#include "../gaEngine/gaCollisionPoint.h"
#include "../gaEngine/gaDebug.h"
#include "../gaEngine/World.h"

#include "dfComponent.h"
#include "dfConfig.h"
#include "dfModel.h"
#include "dfSprites.h"
#include "dfObject/dfSpriteFME.h"
#include "dfObject/dfSprite/ienergy.h"
#include "dfObject/dfSprite/dfRifle.h"
#include "dfLevel.h"
#include "dfCollision.h"
#include "dfComponent/dfComponentActor.h"
#include "dfComponent/dfComponentLogic.h"

static int g_ids = 0;
static const char* g_className = "Object";

/**
 *
 */
DarkForces::Object::Object(dfModel *source, const glm::vec3& position, float ambient, int type, uint32_t objectID):
	gaEntity(DarkForces::ClassID::_Object, source->name() + "(" + std::to_string(objectID) + ")"),
	m_source(source),
	m_position_lvl(position),
	m_ambient(ambient),
	m_is(type),
	m_objectID(g_ids++)
{
	modelAABB(m_source->modelAABB());
	moveTo(position);
	m_className = g_className;
	addComponent(&m_logic);
}

DarkForces::Object::Object(const std::string& model, const glm::vec3& position) :
	gaEntity(DarkForces::ClassID::_Object, model + "(" + std::to_string(g_ids++) + ")"),
	m_position_lvl(position),
	m_objectID(g_ids)
{
	m_source = static_cast<dfModel*>(g_gaWorld.getModel(model));
	m_className = g_className;
	addComponent(&m_logic);
	modelAABB(m_source->modelAABB());
	moveTo(position);
}

/**
 * create a full object from a saved state
 */
DarkForces::Object::Object(flightRecorder::DarkForces::Object* record) :
	gaEntity(&record->entity)
{
	m_className = g_className;
	loadState((flightRecorder::Entity*)record);
}

/**
 * Check the name of th associated WAX
 */
bool DarkForces::Object::named(std::string name)
{
	return m_source->named(name);
}

/**
 * Stack up logics
 */
void DarkForces::Object::logic(uint32_t logic)
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
}

/**
 * Do we check collision ?
 */
bool DarkForces::Object::collision(void)
{
	return m_source->collision();
}

/**
 * test the nature of the object
 */
bool DarkForces::Object::is(int type)
{
	return m_is == type;
}

/**
 * test the logic
 */
bool DarkForces::Object::isLogic(uint32_t logic)
{
	return (m_logics & logic) != 0;
}

int DarkForces::Object::difficulty(void)
{
	return m_difficulty + 3;
}

/**
 * get the name of the model the object is based on
 */
const std::string& DarkForces::Object::model(void)
{
	return m_source->name();
}

/**
 * Update the object position (given in level space) and update the worldboundingBox(in gl space)
 */
void DarkForces::Object::moveTo(const glm::vec3& pposition)
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
bool DarkForces::Object::update(time_t t)
{
	return false;
}

/**
 * object to drop in the scene at the current position
 */
void DarkForces::Object::drop(uint32_t logic, uint32_t value)
{
	DarkForces::Object* obj=nullptr;

	// constructor of a sprite expects a level space
	glm::vec3 p;
	dfLevel::gl2level(position(), p);

	// randomly drop around the object
	float x = m_radius / (rand() % 10);
	float y = m_radius / (rand() % 10);

	p.x += x;
	p.y += y;

	switch (logic) {
	case dfLogic::DEAD_MOUSE:
		obj = new DarkForces::Sprite::FME("DEDMOUSE.FME", p, 1.0f);
		obj->hasCollider(false);
		break;
	case dfLogic::ITEM_BATTERY:
		obj = new DarkForces::Sprite::FME("IBATTERY.FME", p, 1.0f);
		obj->hasCollider(true);
		break;
	case dfLogic::ITEM_RIFLE:
		obj = new DarkForces::Sprite::Rifle(p, 1.0f, value);
		break;
	case dfLogic::ITEM_POWER:
		obj = new DarkForces::Sprite::FME("IPOWER.FME", p, 1.0f);
		obj->hasCollider(true);
		break;
	case dfLogic::ITEM_ENERGY:
		obj = new IEnergy(p, 1.0f, value);
		break;
	case dfLogic::RED_KEY:
		obj = new DarkForces::Sprite::FME("IKEYR.FME", p, 1.0f);
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
void DarkForces::Object::updateWorldAABB(void)
{
	// extract the radius from the AABB
	m_radius = std::max(abs(m_worldBounding.m_p1.x - m_worldBounding.m_p.x), abs(m_worldBounding.m_p1.z - m_worldBounding.m_p.z)) / 2.0f;

	gaEntity::updateWorldAABB();
}

/**
 * populate the super sector if it is not there
 */
dfSuperSector* DarkForces::Object::superSector(void)
{
	if (m_supersector == nullptr) {
		m_supersector = m_sector->supersector();
	}

	return m_supersector;
}

/**
 * extended collision test after a successful AABB collision
 * consider the object is a cylinder
 */
bool DarkForces::Object::checkCollision(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::list<gaCollisionPoint>& collisions)
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
			gaDebugLog(FULL_DEBUG, "Object::checkCollision", message);
#endif
		}
		if (aabb.m_p1.y > m_worldBounding.m_p.y && aabb.m_p1.y < m_worldBounding.m_p1.y) {
			intersection.y = m_worldBounding.m_p.y;
			collisions.push_back(gaCollisionPoint(fwCollisionLocation::TOP, intersection, nullptr));
#ifdef _DEBUG
			std::string message = " TOP z=" + std::to_string(intersection.y);
			gaDebugLog(LOW_DEBUG, "Object::checkCollision", message);
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
		gaDebugLog(LOW_DEBUG, "Object::checkCollision", message);
#endif
	}
	return true;
}

/**
 * return a record of the entity state (for debug)
 */
uint32_t DarkForces::Object::recordState(void* r)
{
	gaEntity::recordState(r);
	flightRecorder::DarkForces::Object* record = (flightRecorder::DarkForces::Object*)r;

	record->entity.classID = flightRecorder::TYPE::DF_ENTITY_OBJECT;
	record->entity.size = sizeof(flightRecorder::DarkForces::Object);

	record->is = m_is;
	record->logics = m_logics;
	record->difficulty = m_difficulty;
	record->ambient = m_ambient;
	record->radius = m_radius;
	record->height = m_height;

	record->position_level = m_position_lvl;
	strncpy_s(record->model, m_source->name().c_str(), sizeof(record->model));

	if (m_sector) {
		strncpy_s(record->sector, m_sector->name().c_str(), sizeof(record->sector));
	}
	else {
		record->sector[0];
	}

	return record->entity.size;
}

/**
 * reload an entity state from a record
 */
void DarkForces::Object::loadState(void* r)
{
	flightRecorder::DarkForces::Object* record = (flightRecorder::DarkForces::Object*)r;
	gaEntity::loadState(&record->entity);

	m_is = record->is;
	m_logics = record->logics;
	m_difficulty = record->difficulty;
	m_ambient = record->ambient;
	m_radius = record->radius;
	m_height = record->height;

	m_source = static_cast<dfModel*>(g_gaWorld.getModel(record->model));
	m_sector = static_cast<dfSector*>(g_gaWorld.getEntity(record->sector));

	m_position_lvl = record->position_level;
}

static std::map<uint32_t, const char*> debugLogic = {
	{1, "SCENERY"},
	{2, "ANIM"},
	{4, "OFFICER"},
	{8, "COMMANDO"},
	{16, "TROOP"},
	{32, "RED_KEY"},
	{64, "INTDROID"},
	{128, "ITEM_SHIELD"},
	{256, "ITEM_ENERGY"},
	{512, "LIFE"},
	{1024, "REVIVE"},
	{2048, "MOUSEBOT"},
	{4096, "KEY_TRIGGER"},
	{8192, "ITEM_RIFLE"},
	{16384, "ITEM_POWER"},
	{32768, "ITEM_BATTERY"},
	{65536, "DEAD_MOUSE"}
};

/**
 * Add dedicated component debug the entity
 */
void DarkForces::Object::debugGUIChildClass(void)
{
	gaEntity::debugGUIChildClass();
	static char tmp[64];
	sprintf_s(tmp, "%s##%d", g_className, m_entityID);

	if (ImGui::TreeNode(tmp)) {
		std::string sLogic = "";
		for (auto& logic : debugLogic) {
			if (m_logics & logic.first) {
				sLogic.append("|");
				sLogic.append(logic.second);
			}
		}
		ImGui::Text("Logic: %s", sLogic.c_str());
		ImGui::Text("Ambient: %f", m_ambient);
		ImGui::Text("Radius: %f", m_radius);
		ImGui::Text("LevelPos %.2f %.2f %.2f", m_position_lvl.x, m_position_lvl.y, m_position_lvl.z);
		ImGui::TreePop();
	}
}

DarkForces::Object::~Object()
{
}
