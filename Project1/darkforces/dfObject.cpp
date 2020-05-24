#include "dfObject.h"

#include "dfModel.h"
#include "dfSprites.h"
#include "dfModel/df3DO.h"
#include "../framework/fwScene.h"
#include "dfLevel.h"

dfObject::dfObject(dfModel *source, glm::vec3& position, float ambient, int type):
	m_source(source),
	m_position(position),
	m_ambient(ambient),
	m_is(type)
{
}

/**
 * Check the name of th associated WAX
 */
bool dfObject::named(std::string name)
{
	return m_source->named(name);
}

/**
 * test the nature of the object
 */
bool dfObject::is(int type)
{
	return m_is == type;
}

/**
 * test the logic
 */
bool dfObject::isLogic(int logic)
{
	return (m_logics & logic) != 0;
}

int dfObject::difficulty(void)
{
	return m_difficulty + 3;
}

/**
 * get the name of the model the object is based on
 */
std::string& dfObject::model(void)
{
	return m_source->name();
}

/**
 * Animate the object frame
 */
bool dfObject::update(time_t t)
{
	return false;
}

/**
 * Stack up logics
 */
void dfObject::logic(int logic)
{
	m_logics |= logic;
}

void dfObject::add2scene(fwScene* scene)
{
	df3DO* model = (df3DO *)m_source;
	if (model) {
		glm::vec3 gl;
		dfLevel::level2gl(m_position, gl);

		model->add2scene(scene, gl);
	}
}

/**
 * Record the rotation axe
 */
void dfObject::animRotationAxe(int axe)
{
	switch (axe) {
	case 8: 
		m_animRotationAxe = glm::vec3(1, 0, 0);
		break;
	case 16:
		m_animRotationAxe = glm::vec3(0, 0, 1);
		break;
	case 32:
		m_animRotationAxe = glm::vec3(0, 1, 0);
		break;
	default:
		std::cerr << "dfObject::animRotationAxe unknown rotation flag " << axe << std::endl;
	}
}

/**
 * Record rotation speed
 */
void dfObject::animRotationSpeed(float s)
{
	m_aniRotationSpeed = s;
}

dfObject::~dfObject()
{
}
