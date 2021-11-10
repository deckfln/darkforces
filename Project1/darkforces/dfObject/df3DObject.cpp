#include "df3DObject.h"

#include "../../gaEngine/World.h"
#include "../dfModel/df3DO.h"

static const char* g_className = "df3D";

void DarkForces::Anim::ThreeD::init(const std::string& model)
{
	m_className = g_className;

	df3DO* tdo = (df3DO*)g_gaWorld.getModel(model);

	physical(false);	// in dark forces, 3D objects can be traversed
	set_scale(glm::vec3(0.1f));

	tdo->clone(m_componentMesh);
	m_componentMesh.set_scale(0.10f);
	addComponent(&m_componentMesh);

	addComponent(&m_anim);
	Object::moveTo(m_position_lvl);
}

DarkForces::Anim::ThreeD::ThreeD(const std::string& model, const glm::vec3& p, float ambient):
	DarkForces::Object(model, p)
{
	init(model);
}

DarkForces::Anim::ThreeD::ThreeD(const std::string& model, const glm::vec3& p, float ambient, uint32_t objectID) :
	DarkForces::Object(model, p, objectID)
{
	init(model);
}
