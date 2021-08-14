#include "dfObject3D.h"

#include <imgui.h>

#include "../../framework/fwMesh.h"
#include "../../framework/fwScene.h"

#include "../../gaEngine/gaBoundingBoxes.h"

#include "../dfFileSystem.h"
#include "../dfModel/df3DO.h"
#include "../dfLevel.h"
#include "../dfVue.h"

dfObject3D::dfObject3D(df3DO* threedo, const glm::vec3& position, float ambient, uint32_t objectID):
	dfObject(threedo, position, ambient, OBJECT_3DO, objectID)
{
	physical(false);	// in dark forces, 3D objects can be traversed
	set_scale(glm::vec3(0.1f));

	addComponent(&m_componentLogic);
	addComponent(&m_componentMesh);

	threedo->clone(m_componentMesh);
	m_componentMesh.set_scale(0.10f);

	dfObject::moveTo(m_position_lvl);
}

dfObject3D::dfObject3D(flightRecorder::DarkForces::Object3D* record) :
	dfObject(&record->object)
{
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
	if (m_logics & dfLogic::ANIM) {

		if (m_vue != nullptr) {
			// follow a path
			glm::mat4* mat4x4 = m_vue->nextFrame(t);
			if (mat4x4 == nullptr) {
				mat4x4 = m_vue->firstFrame(m_lastFrame);
				m_lastFrame = t;
			}
			m_componentMesh.worldMatrix(mat4x4);
			m_worldBounding.apply(m_source->modelAABB(), *mat4x4);
		}
		else {
			// rotate the object
			glm::vec3 r = m_animRotationAxe * m_aniRotationSpeed * (float)t;
			sendInternalMessage(gaMessage::ROTATE, gaMessage::Flag::ROTATE_BY, &r);
		}

		return true;
	}
	return false;
}

/**
 * use the current state of update the world AABB
 *
void dfObject3D::updateWorldAABB(void)
{
	// take the opportunity to update the world bounding box
	m_worldBounding.transform(m_source->modelAABB(), m_position, m_rotation, glm::vec3(0.10f, 0.10f, 0.10f));
}
*/

/**
 * Deal with animation messages
 */
void dfObject3D::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::TIMER:
		if (update(message->m_delta)) {
			sendDelayedMessage(gaMessage::TIMER);
		}
		break;

	case gaMessage::WORLD_INSERT:
		// trigger the animation
		sendDelayedMessage(gaMessage::TIMER);
		break;
	}
	dfObject::dispatchMessage(message);
}

/**
 * return a record of the entity state (for debug)
 */
void* frCreate_df_Object3D(void* record) {
	return new dfObject3D((flightRecorder::DarkForces::Object3D*)record);
}

/**
 * return a record of the entity state (for debug)
 */
uint32_t dfObject3D::recordState(void* r)
{
	dfObject::recordState(r);
	flightRecorder::DarkForces::Object3D* record = (flightRecorder::DarkForces::Object3D*)r;

	record->object.entity.classID = flightRecorder::TYPE::DF_ENTITY_OBJECT;
	record->object.entity.size = sizeof(flightRecorder::DarkForces::Object3D);

	record->lastFrame = m_lastFrame;
	record->vue = false;

	if (m_vue) {
		record->vue = true;
		record->currentVueFrame = m_vue->currentFrame();
	}
	return record->object.entity.size;
}

/**
 * reload an entity state from a record
 */
void dfObject3D::loadState(flightRecorder::Entity* r)
{
	dfObject::loadState(r);
	flightRecorder::DarkForces::Object3D* record = (flightRecorder::DarkForces::Object3D*)r;
	m_lastFrame= record->lastFrame;

	if (record->vue) {
		m_vue->currentFrame(record->currentVueFrame);
		update(0);
	}
}

/**
 * Add dedicated component debug the entity
 */
void dfObject3D::debugGUIChildClass(void)
{
	dfObject::debugGUIChildClass();

	if (ImGui::TreeNode("dfObject3D")) {
		ImGui::Text("frame: %d", m_lastFrame);
		if (m_vue) {
			ImGui::Text("VUE frame: %d", m_vue->currentFrame());
		}
		ImGui::TreePop();
	}
}

dfObject3D::~dfObject3D()
{
	if (m_mesh) {
		delete m_mesh;
	}
}
