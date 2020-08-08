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
#include "../darkforces/dfModel/dfWAX.h"
#include "../darkforces/dfSprites.h"
#include "../darkforces/dfObject/dfSpriteAnimated.h"
#include "../darkforces/dfObject/dfBulletExplode.h"

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
		g_blaster = new fwGeometryCylinder(bullet_radius, bullet_length, 8, -1);
		g_blaster->makeStatic();
	}
	// the AABOX is just the direction vector
	m_modelAABB = g_blaster->aabbox();

	// change the collider to a geometry
	m_collider.set(&m_modelAABB, &m_worldMatrix, &m_inverseWorldMatrix);

	m_componentMesh.set(g_blaster, &g_basic);
	addComponent(&m_componentMesh);

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

	sendInternalMessage(GA_MSG_ROTATE, 1, (void*)&quaternion);
	sendInternalMessage(GA_MSG_MOVE, 0, (void*)&position);

	// kick start animation
	sendDelayedMessage(GA_MSG_TIMER, 0, nullptr);
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
	case GA_MSG_MOVE_TO:
		// move request was accepted
		sendDelayedMessage(GA_MSG_TIMER, 0, nullptr);
		break;

	case GA_MSG_COLLIDE: {
		// add an impact sprite
		// constructor of a sprite expects a level space
		glm::vec3 p;
		dfLevel::gl2level(position(), p);
		dfBulletExplode* impact = new dfBulletExplode(p, 1.0f);
		g_gaWorld.addClient(impact);

		// if hit an entity
		if (message->m_value == 0) {
			// on collision, inform the target it was hit with the energy of the bullet
			sendMessage(message->m_server, DF_MESSAGE_HIT_BULLET, 10, nullptr);
			gaDebugLog(REDUCED_DEBUG, "dfBullet::dispatchMessage", "hit entity");
		}
		else {
			gaDebugLog(LOW_DEBUG, "dfBullet::dispatchMessage", "hit wall");
		}
		// drop the bullet
		sendMessageToWorld(GA_MSG_DELETE_ENTITY, 0, nullptr);
		break;
	}

	case GA_MSG_TIMER:
		m_time += message->m_delta;

		if (m_time < bullet_life) {
			glm::vec3 direction = (m_direction * (float)message->m_delta / bullet_speed);
			m_futurePosition = direction;
			sendMessageToWorld(GA_MSG_WANT_TO_MOVE, GA_MSG_WANT_TO_MOVE_LASER, &m_futurePosition);
		}
		else {
			// get the bullet deleted after 5s
			sendMessageToWorld(GA_MSG_DELETE_ENTITY, 0, nullptr);
			gaDebugLog(LOW_DEBUG, "dfBullet::dispatchMessage", "remove bullet");
		}
		break;
	}

	gaEntity::dispatchMessage(message);
}

dfBullet::~dfBullet()
{
	delete m_animate_msg;
}
