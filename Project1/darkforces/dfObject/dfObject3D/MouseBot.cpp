#include "MouseBot.h"

#include "../../dfVOC.h"

static const char* g_className = "dfMouseBot";
static const uint32_t MouseEeek = 1024;

DarkForces::Anim::MouseBot::MouseBot(const std::string& model, const glm::vec3& p, float ambient, uint32_t objectID) :
	DarkForces::Anim::ThreeD(model, p, ambient, objectID)
{
	m_className = g_className;
	m_physical = true;

	addComponent(&m_ia);
	addComponent(&m_actor);
	addComponent(&m_sound);

	m_actor.setClass("MOUSEBOT.3DO");
	m_sound.addSound(DarkForces::Component::Actor::Sound::DIE, loadVOC("EEEK-3.VOC")->sound());
	m_sound.addSound(MouseEeek, loadVOC("EEEK-1.VOC")->sound());
	m_anim.active(false);	// deactivate the animation component

	// cylinders run in world space, so adapt from the model space scale
	const glm::vec3& scale = get_scale();
	float r;

	r = (std::max(m_modelAABB.m_p1.x - m_modelAABB.m_p.x, m_modelAABB.m_p1.z - m_modelAABB.m_p.z))/2.0f;
	m_cylinder.height(m_modelAABB.height());
	m_cylinder.radius(m_modelAABB.height() / 2.0f);
	m_collider.set(&m_cylinder, &m_worldMatrix, &m_inverseWorldMatrix, &m_modelAABB);
}