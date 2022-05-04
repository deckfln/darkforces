#include "df3DObject.h"

#include "../../gaEngine/World.h"
#include "../dfModel/df3DO.h"

static const char* g_className = "df3D";

void DarkForces::Anim::ThreeD::init(const std::string& model)
{
	m_classID |= DarkForces::ClassID::_3DO;
	m_className = g_className;

	df3DO* tdo = (df3DO*)g_gaWorld.getModel(model);

	physical(false);	// in dark forces, 3D objects can be traversed
	set_scale(glm::vec3(0.1f));

	tdo->clone(m_componentMesh);

	m_uniformAmbient = new fwUniform("ambient", &m_ambient);
	m_componentMesh.addUniform(m_uniformAmbient);
	m_componentMesh.set_scale(0.10f);
	addComponent(&m_componentMesh);

	addComponent(&m_anim);
	Object::moveTo(m_position_lvl);
}

/**
 *
 */
void DarkForces::Anim::ThreeD::onMove(gaMessage* message)
{
	dfSector* currentSector = nullptr;
	const glm::vec3& p = message->m_v3value;

	currentSector = static_cast<dfLevel*>(g_gaLevel)->findSector(position());
	if (currentSector) {
		m_ambient = currentSector->ambient() / 32.0;
	}
}

/**
 *
 */
DarkForces::Anim::ThreeD::ThreeD(const std::string& model, const glm::vec3& p, float ambient):
	DarkForces::Object(model, p)
{
	init(model);
}

/**
 *
 */
DarkForces::Anim::ThreeD::ThreeD(const std::string& model, const glm::vec3& p, float ambient, uint32_t objectID) :
	DarkForces::Object(model, p, objectID)
{
	init(model);
}

/**
 *
 */
void DarkForces::Anim::ThreeD::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::MOVE:
		onMove(message);
		break;
	}

	Object::dispatchMessage(message);
}