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

#include "../darkforces/dfSuperSector.h"
#include "../darkforces/dfObject/dfSpriteAnimated.h"
#include "../darkforces/dfModel/dfWAX.h"
#include "../darkforces/dfSprites.h"

const float bullet_length = 0.5f;
const float bullet_radius = 0.01f;
const float bullet_speed = 500.0f;
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

	m_position += m_direction *132.0f/ bullet_speed;
	m_mesh->translate(m_position);

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
  * trigger when the bullet is added to the world
  * Also add the mesh to the scene
  */
void dfBullet::OnWorldInsert(void)
{
	g_gaWorld.add2scene(this);
}

/**
 * move the bullet forward
 */
void dfBullet::dispatchMessage(gaMessage* message)
{
	std::list<gaEntity*> entities;
	std::list<dfSuperSector*> sectors;
	gaEntity* nearest_entity = nullptr;
	dfSuperSector* nearest_sector = nullptr;
	float distance = 99999999.0f;
	float distance_sector = 99999999.0f;
	float d;

	switch (message->m_action) {
	case GA_MSG_TIMER:
		m_time += message->m_delta;

		if (m_time < bullet_life) {
			glm::vec3 direction = (m_direction * (float)message->m_delta / bullet_speed);
			std::list<gaCollisionPoint> collisions;

			g_gaWorld.findAABBCollision(m_worldBounding, entities, sectors);

			// do an AABB collision against AABB collision with entities
			for (auto entity : entities) {
				// only test entities that can physically checkCollision
				if (entity->physical()) {
					d = this->distanceTo(entity);
					if (d < distance) {
						nearest_entity = entity;
						distance = d;
					}
				}
			}

			// do an segment collision against the sectors triangles
			for (auto sector : sectors) {
				sector->collisionSegmentTriangle(m_position, m_position + direction, collisions);

				for (auto c : collisions) {
					d = this->distanceTo(c.m_position);
					if (d < distance_sector) {
						nearest_sector = sector;
						distance_sector = d;
					}
				}
			}

			// and take action
			if (nearest_entity == nullptr && nearest_sector == nullptr) {
				// next animation
				m_position += direction;
				moveTo(m_position);
				m_mesh->translate(m_position);

				g_gaWorld.pushForNextFrame(m_animate_msg);
			}
			else {
				// add an impact sprite
				// constructor of a sripte expects a level space
				glm::vec3 p;
				dfLevel::gl2level(m_position, p);
				dfSpriteAnimated* impact = new dfSpriteAnimated("BULLEXP.WAX", p, 1.0f);
				impact->state(DF_STATE_ENEMY_MOVE);
				g_gaWorld.addClient(impact);

				// if nearest is an entity
				if (distance < distance_sector) {
					// on collision,, inform the target it was hit with the energy of the bullet
					g_gaWorld.sendMessage(m_name, nearest_entity->name(), DF_MESSAGE_HIT_BULLET, 10, nullptr);
					gaDebugLog(REDUCED_DEBUG, "dfBullet::dispatchMessage", "hit entity");
				}
				else {
					gaDebugLog(LOW_DEBUG, "dfBullet::dispatchMessage", "hit wall");
				}
				// drop the bullet
				g_gaWorld.sendMessage(m_name, "_world", GA_MSG_DELETE_ENTITY, 0, nullptr);
			}
		}
		else {
			// get the bullet deleted after 5s
			g_gaWorld.sendMessage(m_name, "_world", GA_MSG_DELETE_ENTITY, 0, nullptr);
			gaDebugLog(LOW_DEBUG, "dfBullet::dispatchMessage", "remove bullet");
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
