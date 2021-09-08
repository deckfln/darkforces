#include "MouseBot.h"

using namespace DarkForces;

static const char* g_className = "dfMouseBot";

MouseBot::MouseBot(df3DO* threedo, const glm::vec3& position, float ambient, uint32_t objectID) :
	dfObject3D(threedo, position, ambient, objectID)
{
	m_className = g_className;

	physical(true);
	addComponent(&m_ia);

	// cylinders run in world space, so adapt from the model space scale
	const glm::vec3& scale = get_scale();

	m_cylinder.height(m_modelAABB.height() * scale.y);
	m_cylinder.radius(((m_modelAABB.m_p1.x - m_modelAABB.m_p.x) / 2.0f) * scale.x);
	m_collider.set(&m_cylinder, &m_worldMatrix, &m_inverseWorldMatrix, &m_modelAABB);
}

MouseBot::~MouseBot()
{
}
