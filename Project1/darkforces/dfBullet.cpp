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

static int g_bulletID = 0;
static glm::vec4 g_red(1.0, 0.0, 0.0, 1.0);
static fwMaterialBasic g_basic(&g_red);
static fwGeometryCylinder *g_blaster=nullptr;

dfBullet::dfBullet(const glm::vec3& position, const glm::vec3& direction):
	gaEntity(DF_ENTITY_BULLET, "bullet("+std::to_string(g_bulletID++)+")", position),
	m_direction(direction)
{
	// the AABOX is just the direction vector
	glm::vec3 d = direction * 0.1f;
	m_modelAABB = fwAABBox(glm::vec3(0), d);
	updateWorldAABB();

	// create a mesh for the blaster
	if (g_blaster == nullptr) {
		g_blaster = new fwGeometryCylinder(0.01f, 0.1f);
	}
	m_mesh = new fwMesh(g_blaster, &g_basic);
	m_mesh->set_name(m_name);

	glm::vec3 p = m_position + m_direction / 1000.0f;
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
		m_position += (m_direction * (float)message->m_delta / 1000.0f);
		m_mesh->translate(m_position);

		// next animation
		g_gaWorld.pushForNextFrame(m_animate_msg);
		break;
	}
}

dfBullet::~dfBullet()
{
	delete m_mesh;
	delete m_animate_msg;
}
