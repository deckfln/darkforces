#include "dfCAnim3D.h"

#include <imgui.h>

#include "../../framework/fwMesh.h"
#include "../../framework/fwScene.h"

#include "../../gaEngine/gaBoundingBoxes.h"

#include "../dfFileSystem.h"
#include "../dfModel/df3DO.h"
#include "../dfLevel.h"
#include "../dfVue.h"
#include "../dfComponent.h"

#include "../flightRecorder/frAnim3D.h"

DarkForces::Component::Anim3D::Anim3D(void) :
	gaComponent(DF_COMPONENT_3DO)
{
}

DarkForces::Component::Anim3D::Anim3D(dfVue* vue) :
	gaComponent(DF_COMPONENT_3DO),
	m_vue(vue)
{
}

DarkForces::Component::Anim3D::Anim3D(const std::string& vue, const std::string& component) :
	gaComponent(DF_COMPONENT_3DO),
	m_vue(new dfVue(g_dfFiles, vue, component))
{
}

void DarkForces::Component::Anim3D::set(dfVue* vue)
{
	m_vue = vue;
}

void DarkForces::Component::Anim3D::set(const std::string& vue, const std::string& component)
{
	m_vue = new dfVue(g_dfFiles, vue, component);
}

/**
 * animated the object
 */
void DarkForces::Component::Anim3D::onTimer(gaMessage* message)
{
	time_t t = message->m_delta;
	if (m_vue != nullptr) {
		// follow a path
		glm::mat4* mat4x4 = m_vue->nextFrame(t);
		if (mat4x4 == nullptr) {
			mat4x4 = m_vue->firstFrame(m_lastFrame);
			m_lastFrame = t;
		}
		m_entity->sendMessage(gaMessage::MOVE_ROTATE, 0, (void*)mat4x4);
	}
	else {
		// rotate the object
		const glm::vec3 r = m_animRotationAxe * m_aniRotationSpeed * (float)t;
		m_entity->sendMessage(gaMessage::ROTATE, gaMessage::Flag::ROTATE_BY, r);
	}
}

/**
 * Change rotation axe/speed
 */
void DarkForces::Component::Anim3D::onRotationChange(gaMessage* message)
{
	switch (message->m_value) {
	case gaMessage::Flag::ROTATE_SPEED:
		// Speed is the speed at which the 3D object rotates from -999 (max anti-clockwise) to 999 (max clockwise)
		// measured from the death mesh rotation: 7260ms for 1 turn => 7260ms for 2000 rotations => 0.27r/ms
		//		death star speed = 50. so 50 / 180 => 0.27r/ms
		m_aniRotationSpeed = message->m_fvalue / 180.0f * 0.0031415f;
		break;

	case gaMessage::Flag::ROTATE_AXE:
		switch ((uint32_t)message->m_fvalue) {
		case 8:
			m_animRotationAxe = glm::vec3(1, 0, 0);
			break;
		case 16:
			m_animRotationAxe = glm::vec3(0, 1, 0);
			break;
		case 32:
			m_animRotationAxe = glm::vec3(0, 0, 1);
			break;
		}
		break;
	}

}

/**
 * change the pause status end of end of a loop
 */
void DarkForces::Component::Anim3D::onAnimPause(gaMessage* message)
{
	// change the status of the looping animation
	m_pause = (message->m_value != 0);
}

/**
 * load an new animation VUE file
 */
void DarkForces::Component::Anim3D::onAnimVue(gaMessage* message)
{
	const char* file = (const char*)&message->m_data;
	char* vue = (char *)strchr(file, '/');
	if (vue == nullptr) {
		return;
	}
	*vue = 0;
	vue++;

	if (m_vue) {
		delete m_vue;
	}

	m_vue = new dfVue(g_dfFiles, file, vue);
}

/**
 * Deal with animation messages
 */
void DarkForces::Component::Anim3D::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::TIMER:
		onTimer(message);
		break;

	case gaMessage::Action::ROTATE:
		onRotationChange(message);
		break;

	case gaMessage::WORLD_INSERT:
		// trigger the animation
		m_entity->timer(true);
		break;

	case DarkForces::Message::ANIM_PAUSE:
		onAnimPause(message);
		break;

	case DarkForces::Message::ANIM_VUE:
		onAnimVue(message);
		break;
	}
	gaComponent::dispatchMessage(message);
}

//*************************** Flight recorder ******************************

inline uint32_t DarkForces::Component::Anim3D::recordSize(void)
{
	return sizeof(flightRecorder::DarkForces::Anim3D);
}

uint32_t DarkForces::Component::Anim3D::recordState(void* r)
{
	flightRecorder::DarkForces::Anim3D* record = (flightRecorder::DarkForces::Anim3D*)r;

	record->id = m_id;
	record->size = sizeof(flightRecorder::DarkForces::Anim3D);

	record->lastFrame = m_lastFrame;
	record->vue = false;

	if (m_vue) {
		record->vue = true;
		record->currentVueFrame = m_vue->currentFrame();
	}
	return record->size;
}

uint32_t DarkForces::Component::Anim3D::loadState(void* r)
{
	flightRecorder::DarkForces::Anim3D* record = (flightRecorder::DarkForces::Anim3D*)r;
	m_lastFrame = record->lastFrame;

	if (record->vue) {
		m_vue->currentFrame(record->currentVueFrame);
		onTimer(0);
	}
	return record->size;
}

//*************************** Debugger ******************************

/**
 * Add dedicated component debug the entity
 */
void DarkForces::Component::Anim3D::debugGUIinline(void)
{
	if (ImGui::TreeNode("3DO")) {
		ImGui::Text("frame: %d", m_lastFrame);
		if (m_vue) {
			ImGui::Text("VUE frame: %d", m_vue->currentFrame());
		}
		ImGui::TreePop();
	}
}

/**
 *
 */
DarkForces::Component::Anim3D::~Anim3D(void)
{
	if (m_vue) {
		delete m_vue;
	}
}
