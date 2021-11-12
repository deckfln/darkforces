#include "MouseBot.h"

static const char* g_className = "dfMouseBot";

DarkForces::Anim::MouseBot::MouseBot(const std::string& model, const glm::vec3& p, float ambient, uint32_t objectID) :
	DarkForces::Anim::ThreeD(model, p, ambient, objectID)
{
	m_className = g_className;
	m_physical = true;

	addComponent(&m_ia);

	// cylinders run in world space, so adapt from the model space scale
	const glm::vec3& scale = get_scale();
	float r;

	r = (std::max(m_modelAABB.m_p1.x - m_modelAABB.m_p.x, m_modelAABB.m_p1.z - m_modelAABB.m_p.z))/2.0f;
	m_cylinder.height(m_modelAABB.height());
	m_cylinder.radius(m_modelAABB.height() / 2.0f);
	m_collider.set(&m_cylinder, &m_worldMatrix, &m_inverseWorldMatrix, &m_modelAABB);
}