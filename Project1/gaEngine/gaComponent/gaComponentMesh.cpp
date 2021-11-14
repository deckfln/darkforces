#include "gaComponentMesh.h"

#include <imgui.h>

#include "../../alEngine/alSound.h"
#include "../../framework/fwMesh.h"

#include "../World.h"
#include "../gaEntity.h"
#include "../flightRecorder/frCMesh.h"

using namespace GameEngine;

ComponentMesh::ComponentMesh(void):
	gaComponent(gaComponent::MESH)
{
}

ComponentMesh::ComponentMesh(fwMesh* mesh):
	gaComponent(gaComponent::MESH),
	m_mesh(mesh)
{
}

void GameEngine::ComponentMesh::clone(fwMesh* source)
{
	if (m_mesh == nullptr) {
		m_mesh = new fwMesh();
	}

	m_mesh->clone(source);
}

void GameEngine::ComponentMesh::set(fwGeometry* geometry, fwMaterial* material)
{
	if (m_mesh == nullptr) {
		m_mesh = new fwMesh();
	}

	m_mesh->set(geometry, material); 
}

/**
 * move the mesh
 */
void GameEngine::ComponentMesh::onMove(gaMessage* message)
{
	if (message->m_extra != nullptr) {
		glm::vec3* position = (glm::vec3*)message->m_extra;
		m_mesh->translate(position);
	}
	else {
		m_mesh->translate(m_entity->position());
		m_mesh->rotate(m_entity->quaternion());
		m_mesh->set_scale(m_entity->get_scale());
		//worldMatrix(m_entity->worldMatrix(),				m_entity->inverseWorldMatrix()	);
	}
}

/**
 * rotate the mesh
 */
void GameEngine::ComponentMesh::onRotate(gaMessage* message)
{
	if (message->m_extra == nullptr) {
		switch (message->m_value) {
		case gaMessage::Flag::ROTATE_VEC3:
			m_mesh->rotate(message->m_v3value);
			break;
		case gaMessage::Flag::ROTATE_BY:
			m_mesh->rotateBy(message->m_v3value);
			break;
		}
	}
	else {
		switch (message->m_value) {
		case gaMessage::Flag::ROTATE_VEC3:
			m_mesh->rotate((glm::vec3*)message->m_extra);
			break;
		case gaMessage::Flag::ROTATE_QUAT:
			m_mesh->rotate((glm::quat*)message->m_extra);
			break;
		case gaMessage::Flag::ROTATE_BY:
			m_mesh->rotateBy((glm::vec3*)message->m_extra);
			break;
		}
	}
}

/**
 * when the object is first injected in the world
 */
void GameEngine::ComponentMesh::onWorldInsert(gaMessage* message)
{
	g_gaWorld.add2scene(m_mesh);
}

/**
 * when the object is removed from the world
 */
void GameEngine::ComponentMesh::onWorldRemove(gaMessage* message)
{
	g_gaWorld.remove2scene(m_mesh);
}

/**
 * handle fwMesh actions
 */
void ComponentMesh::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::MOVE:
		onMove(message);
		break;

	case gaMessage::Action::MOVE_ROTATE:
		m_mesh->worldMatrix((glm::mat4*)message->m_extra);
		break;

	case gaMessage::ROTATE:
		onRotate(message);
		break;
	
	case gaMessage::PLAY_SOUND: {
		 // Start playing a sound or check if it plays
		alSound* voc = (alSound*)message->m_extra;
		if (voc)
			m_mesh->play(voc);
		break;
	}
	case gaMessage::STOP_SOUND: {
		// Stop playing a sound (or all sound if nullptr)
		alSound* voc = (alSound*)message->m_extra;
		if (voc)
			m_mesh->stop(voc);
		break;
	}

	case gaMessage::WORLD_INSERT:
		onWorldInsert(message);
		break;

	case gaMessage::WORLD_REMOVE:
		onWorldRemove(message);
		break;
	}
}

//****************************************************

/**
 * display the component in the debugger
 */
void ComponentMesh::debugGUIinline(void)
{
	if (ImGui::TreeNode("Mesh")) {
		m_mesh->debugGUIChildClass();
		ImGui::TreePop();
	}
}

//*********************************************

inline uint32_t GameEngine::ComponentMesh::recordSize(void)
{
	return sizeof(flighRecorder::GameEngine::CMesh);
}

uint32_t GameEngine::ComponentMesh::recordState(void* r)
{
	flighRecorder::GameEngine::CMesh* record = static_cast<flighRecorder::GameEngine::CMesh*>(r);

	record->size = sizeof(flighRecorder::GameEngine::CMesh);
	m_mesh->recordState(&record->object3D);

	return record->size;
}

uint32_t GameEngine::ComponentMesh::loadState(void* r)
{
	flighRecorder::GameEngine::CMesh* record = static_cast<flighRecorder::GameEngine::CMesh*>(r);
	m_mesh->loadState(&record->object3D);
	return record->size;
}

ComponentMesh::~ComponentMesh()
{
	delete m_mesh;
}
