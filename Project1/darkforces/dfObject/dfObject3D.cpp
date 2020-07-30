#include "dfObject3D.h"

#include "../../framework/fwMesh.h"
#include "../../framework/fwScene.h"

#include "../../gaEngine/gaBoundingBoxes.h"

#include "../dfFileSystem.h"
#include "../dfModel/df3DO.h"
#include "../dfLevel.h"
#include "../dfVue.h"

dfObject3D::dfObject3D(df3DO* threedo, glm::vec3& position, float ambient):
	dfObject(threedo, position, ambient, OBJECT_3DO)
{
	physical(false);	// in dark forces, 3D objects can be traversed
	addComponent(&m_componentLogic);
	addComponent(&m_componentMesh);

	dfLevel::level2gl(m_position_lvl, m_position);

	threedo->clone(m_componentMesh);
	m_componentMesh.set_scale(0.10f);

	dfObject::moveTo(m_position_lvl);
}

/**
 * Record the rotation axes
 */
void dfObject3D::animRotationAxe(int axe)
{
	switch (axe) {
	case 8:
		m_animRotationAxe = glm::vec3(1, 0, 0);
		break;
	case 16:
		m_animRotationAxe = glm::vec3(0, 1, 0);
		break;
	case 32:
		m_animRotationAxe = glm::vec3(0, 0, 1);
		break;
	default:
		std::cerr << "dfObject::animRotationAxe unknown rotation flag " << axe << std::endl;
	}
}

/**
 * Record rotation speed
 */
void dfObject3D::animRotationSpeed(float s)
{
	// Speed is the speed at which the 3D object rotates from -999 (max anti-clockwise) to 999 (max clockwise)
	// measured from the death mesh rotation: 7260ms for 1 turn => 7260ms for 2000 rotations => 0.27r/ms
	//		death star speed = 50. so 50 / 180 => 0.27r/ms

	m_aniRotationSpeed = s / 180.0f * 0.0031415f;
}

/**
 * Bind the proper animation VUE to the object
 */
void dfObject3D::vue(dfFileSystem *fs, const std::string& vue, const std::string& component)
{
	m_vue = new dfVue(fs, vue, component);
}

/**
 * animated the object
 */
bool dfObject3D::update(time_t t)
{
	if (m_logics & DF_LOGIC_ANIM) {

		if (m_vue != nullptr) {
			// follow a path
			glm::mat4* mat4x4 = m_vue->nextFrame(t);
			if (mat4x4 == nullptr) {
				mat4x4 = m_vue->firstFrame(m_lastFrame);
				m_lastFrame = t;
			}
			m_componentMesh.worldMatrix(*mat4x4);
			m_worldBounding.apply(m_source->bounding(), *mat4x4);
		}
		else {
			// rotate the object
			m_rotation += m_animRotationAxe * m_aniRotationSpeed * (float)t;
			sendInternalMessage(GA_MSG_ROTATE, 0, &m_rotation);
		}

		return true;
	}
	return false;
}

/**
 * use the current state of update the world AABB
 */
void dfObject3D::updateWorldAABB(void)
{
	// take the opportunity to update the world bounding box
	m_worldBounding.transform(m_source->bounding(), m_position, m_rotation, glm::vec3(0.10f, 0.10f, 0.10f));
}

/**
 * Deal with animation messages
 */
void dfObject3D::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case GA_MSG_TIMER:
		if (update(message->m_delta)) {
			g_gaWorld.sendMessageDelayed(m_name, m_name, GA_MSG_TIMER, 0, nullptr);
		}
		break;

	case GA_MSG_WORLD_INSERT:
		// trigger the animation
		g_gaWorld.sendMessageDelayed(m_name, m_name, GA_MSG_TIMER, 0, nullptr);
		break;
	}
	dfObject::dispatchMessage(message);
}

dfObject3D::~dfObject3D()
{
	if (m_mesh) {
		delete m_mesh;
	}
}
