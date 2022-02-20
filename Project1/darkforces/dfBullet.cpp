#include "dfBullet.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <map>
#include <random>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../config.h"

#include "../framework/fwScene.h"
#include "../framework/fwMesh.h"
#include "../framework/fwMaterialBasic.h"
#include "../framework/geometries/fwGeometryCylinder.h"

#include "../gaEngine/World.h"
#include "../gaEngine/gaMessage.h"
#include "../gaEngine/gaCollisionPoint.h"
#include "../gaEngine/gaLevel.h"

#include "dfConfig.h"
#include "weapons.h"
#include "dfSuperSector.h"
#include "dfModel/dfWAX.h"
#include "dfObject/dfBulletExplode.h"
#include "dfVOC.h"
#include "dfSector.h"

const float bullet_length = 0.60f;
const float bullet_radius = 0.005f;
const float bullet_speed = 50.0f;
const int bullet_life = 2000;

static int g_bulletID = 0;
static glm::vec4 g_red(1.0, 0.0, 0.0, 1.0);
static fwMaterialBasic g_basic(&g_red);
static fwGeometryCylinder *g_blaster=nullptr;

static const char* g_className = "dfBullet";

/**
 *
 */
dfBullet::dfBullet(uint32_t damage , const glm::vec3& position, const glm::vec3& direction):
	gaEntity(DarkForces::ClassID::_Bullet, "bullet("+std::to_string(g_bulletID++)+")", position)
{
	init(damage, position, direction);
}

/**
 *
 */
dfBullet::dfBullet(flightRecorder::dfBullet* record):
	gaEntity(&record->entity)
{
	init(record->damage, record->entity.object3D.position, record->direction);
}

/**
 *
 */
void dfBullet::tryToMove(void)
{
	m_transforms.m_position = position() + m_transforms.m_forward;

	sendDelayedMessage(gaMessage::WANT_TO_MOVE,
		gaMessage::Flag::WANT_TO_MOVE_LASER,
		&m_transforms);
}

/**
 *
 */
void dfBullet::init(uint32_t damage, const glm::vec3& position, const glm::vec3& direction)
{
	m_className = g_className;

	m_direction = glm::normalize(direction);
	m_damage = damage;

	// create a mesh for the blaster
	if (g_blaster == nullptr) {
		g_basic.makeStatic();
		g_blaster = new fwGeometryCylinder(bullet_radius, bullet_length, 8, -1);
		g_blaster->makeStatic();
	}

	// change the collider to a geometry
	m_segment.m_start = glm::vec3(0);
	m_segment.m_end = glm::vec3(0.0, 0.0, 1.0) * 33.0f / bullet_speed;
	m_collider.set(&m_segment, &m_worldMatrix, &m_inverseWorldMatrix, &m_modelAABB);

	// the AABOX is just the direction vector multiplied by a 30fps frame
	m_modelAABB.set(m_segment.m_start, m_segment.m_end);

	// convert the direction vector to a quaternion
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right;
	glm::vec3 up;

	if (m_direction != glm::vec3(0)) {
		right = glm::normalize(glm::cross(_up, m_direction));
		up = glm::cross(m_direction, right);
	}
	else {
		up = _up;
	}

	m_transforms.m_forward = glm::vec3(0);
	m_transforms.m_downward = glm::vec3(0);
	m_transforms.m_quaternion = glm::quatLookAt(m_direction, up);
	m_transforms.m_scale = get_scale();
	m_transforms.m_position = position;

	m_gravity = false;	// laser are not affected by gravity
	rotate(m_transforms.m_quaternion);

	// add the mesh component to the entity
	m_componentMesh.set(g_blaster, &g_basic);
	addComponent(&m_componentMesh);
}

/**
 * move the bullet forward
 */
void dfBullet::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::WORLD_INSERT:
		sendDelayedMessage(gaMessage::WANT_TO_MOVE,
			gaMessage::Flag::WANT_TO_MOVE_LASER,
			&m_transforms);
		break;

	case gaMessage::Action::COLLIDE: {
		// add an impact sprite at the collision point
		// constructor of a sprite expects a level space
		glm::vec3 p;
		dfLevel::gl2level(message->m_v3value, p);

		dfBulletExplode* impact = new dfBulletExplode(p, 1.0f);

		g_gaWorld.addClient(impact);

		// on collision, inform the target it was hit with the energy of the bullet
		sendMessage(message->m_server, gaMessage::Action::BULLET_HIT, m_damage, message->m_v3value, m_shooter);
		gaDebugLog(REDUCED_DEBUG, "dfBullet::dispatchMessage", "hit");

		if (m_current) {
			m_current->popAmbient();
			m_current = nullptr;
		}

		// drop the bullet
		sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
		break;
	}

	case gaMessage::Action::MOVE:
		m_animation_time += message->m_delta;

		if (m_animation_time < bullet_life) {
			// change the lighting of the sector
			dfSector* current = static_cast<dfLevel*>(g_gaLevel)->findSector(position());
			if (current != m_current) {
				if (m_current != nullptr) {
					// reset the old sector lightning
					m_current->popAmbient();
				}
				m_current = current;
				m_current->addToAmbient(+10.0f);
			}

			glm::vec3 d = (m_direction * (float)message->m_delta / bullet_speed);
			m_transforms.m_position = position() + d;
			m_transforms.m_forward = -(m_modelAABB.m_p1.z - m_modelAABB.m_p.z) * m_direction;

			sendDelayedMessage(gaMessage::WANT_TO_MOVE,
				gaMessage::Flag::WANT_TO_MOVE_LASER,
				&m_transforms);
		}
		else {
			if (m_current) {
				m_current->popAmbient();
				m_current = nullptr;
			}

			// get the bullet deleted after 5s
			sendMessageToWorld(gaMessage::DELETE_ENTITY, 0, nullptr);
			gaDebugLog(LOW_DEBUG, "dfBullet::dispatchMessage", "remove bullet");
		}
		break;
	}

	gaEntity::dispatchMessage(message);
}

/**
 * return a record of an actor state (for debug)
 */
uint32_t dfBullet::recordState(void* r)
{
	flightRecorder::dfBullet* record = (flightRecorder::dfBullet*)r;
	gaEntity::recordState(&record->entity);
	record->entity.classID = flightRecorder::TYPE::DF_ENTITY_BULLET;
	record->entity.size = sizeof(flightRecorder::dfBullet);
	record->direction = m_direction;
	record->damage = m_damage;

	return record->entity.size;
}

/**
 * reload an actor state from a record
 */
void dfBullet::loadState(void* r)
{
	flightRecorder::dfBullet* record = (flightRecorder::dfBullet*)r;
	gaEntity::loadState(&record->entity);
	m_direction = record->direction;
	m_damage = record->damage;
}

dfBullet::~dfBullet()
{
}
