#include "gaComponentMesh.h"

#include <imgui.h>

#include "../../alEngine/alSound.h"
#include "../../framework/fwMesh.h"

#include "../World.h"
#include "../gaEntity.h"

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
 * handle fwMesh actions
 */
void ComponentMesh::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::MOVE: {
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
		break;
	}
	case gaMessage::ROTATE: {
		switch (message->m_value) {
		case gaMessage::Flag::ROTATE_VEC3:
			m_mesh->rotate((glm::vec3*)message->m_extra);
			break;
		case gaMessage::Flag::ROTATE_QUAT:
			m_mesh->rotate((glm::quat*)message->m_extra);
			break;
		case gaMessage::Flag::ROTATE_BY:
			m_mesh->rotateBy((glm::vec3*)message->m_extra);
		}
		break;
	}
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
		g_gaWorld.add2scene(m_mesh);
		break;

	case gaMessage::WORLD_REMOVE:
		g_gaWorld.remove2scene(m_mesh);
		break;
	}
}

/**
 * display the component in the debugger
 */
void ComponentMesh::debugGUIinline(void)
{
	if (ImGui::TreeNode("Mesh")) {
		ImGui::Text("ID %d", m_mesh->id());
		ImGui::TreePop();
	}
}

ComponentMesh::~ComponentMesh()
{
	delete m_mesh;
}
