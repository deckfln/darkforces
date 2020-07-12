#include "dfBullet.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <string>
#include <glm/vec4.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../config.h"

#include "../framework/fwScene.h"
#include "../framework/fwMesh.h"
#include "../framework/fwMaterialBasic.h"
#include "../framework/geometries/fwGeometryCylinder.h"

#include "../gaEngine/gaWorld.h"
#include "../gaEngine/gaMessage.h"
#include "../gaEngine/gaCollisionPoint.h"

const float bullet_length = 0.5f;
const float bullet_radius = 0.01f;
const float bullet_speed = 250.0f;
const int bullet_life = 2000;

static int g_bulletID = 0;
static glm::vec4 g_red(1.0, 0.0, 0.0, 1.0);
static fwMaterialBasic g_basic(&g_red);
static fwGeometryCylinder *g_blaster=nullptr;

dfBullet::dfBullet(const glm::vec3& position, const glm::vec3& direction):
	gaEntity(DF_ENTITY_BULLET, "bullet("+std::to_string(g_bulletID++)+")", position),
	m_direction(direction)
{
	// create a mesh for the blaster
	if (g_blaster == nullptr) {
		g_basic.makeStatic();
		g_blaster = new fwGeometryCylinder(bullet_radius, bullet_length);
		g_blaster->makeStatic();
	}
	// the AABOX is just the direction vector
	m_modelAABB = g_blaster->aabbox();

	m_mesh = new fwMesh(g_blaster, &g_basic);
	m_mesh->set_name(m_name);

	glm::vec3 p = m_position + m_direction / bullet_speed;
	m_mesh->translate(p);

	// convert the direction vector to a quaternion
	glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right;
	glm::vec3 up;

	if (m_direction != glm::vec3(0)) {
		m_direction = glm::normalize(m_direction);
		right = glm::normalize(glm::cross(_up, m_direction));
		up = glm::cross(m_direction, right);
	}
	else {
		up = _up;
	}

	glm::quat quaternion = glm::quatLookAt(m_direction, up);

	m_mesh->rotate(quaternion);
	m_quaternion = quaternion;

	updateWorldAABB();

	// next animation
	m_animate_msg = new gaMessage(GA_MSG_TIMER);
	m_animate_msg->m_client = m_name;

	// trigger the naimation
	g_gaWorld.pushForNextFrame(m_animate_msg);
}

/**
 * move the bullet forward
 */
void dfBullet::dispatchMessage(gaMessage* message)
{

	switch (message->m_action) {
	case GA_MSG_TIMER:
		m_time += message->m_delta;

		if (m_time < bullet_life) {
			m_position += (m_direction * (float)message->m_delta / bullet_speed);
			moveTo(m_position);
			m_mesh->translate(m_position);

			// check collision with entities

			// get all the entities which AABB checkCollision with the bullet
			std::list<gaEntity*> entities;
			g_gaWorld.findAABBCollision(m_worldBounding, entities);

			gaEntity* nearest_entity=nullptr;
			float distance = 99999999.0f;

			for (auto entity : entities) {
				if (entity == this || entity->name() == "player") {
					continue;
				}
				// only test entities that can physically checkCollision, but still inform the target of the collision
				if (entity->physical()) {
					float d = this->distanceTo(entity);
					if (d < distance) {
						nearest_entity = entity;
						distance = d;
					}
				}
			}
			// and take action
			if (nearest_entity == nullptr) {
				// next animation
				g_gaWorld.pushForNextFrame(m_animate_msg);
			}
			else {
				// on collision,, inform the target it was hit with the energy of the bullet
				g_gaWorld.sendMessage(m_name, nearest_entity->name(), DF_MESSAGE_HIT_BULLET, 10, nullptr);
				// drop the bullet
				g_gaWorld.sendMessage(m_name, "_world", GA_MSG_DELETE_ENTITY, 0, nullptr);
			}
		}
		else {
			// get the bullet deleted after 5s
			g_gaWorld.sendMessage(m_name, "_world", GA_MSG_DELETE_ENTITY, 0, nullptr);
		}
		break;
	}

	gaEntity::dispatchMessage(message);
}

dfBullet::~dfBullet()
{
	delete m_mesh;
	delete m_animate_msg;
}
