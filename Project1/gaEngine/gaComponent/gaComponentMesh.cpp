#include "gaComponentMesh.h"

#include "../../alEngine/alSound.h"

#include "../gaEntity.h"

using namespace GameEngine;

ComponentMesh::ComponentMesh(void):
	gaComponent(GA_COMPONENT_MESH)
{
}

ComponentMesh::ComponentMesh(fwGeometry* _geometry, fwMaterial* _material):
	gaComponent(GA_COMPONENT_MESH),
	m_mesh(_geometry, _material)
{
}

/***
 * clone from an existing fwMesh
 */
void ComponentMesh::clone(fwMesh* mesh)
{
	m_mesh.clone(mesh);
}

/**
 * handle fwMesh actions
 */
void ComponentMesh::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case GA_MSG_MOVE:
		glm::vec3 position = *(glm::vec3*)message->m_extra;
		m_mesh.translate(position);
		break;
	case GA_MSG_ROTATE:
		glm::vec3 rotation = *(glm::vec3*)message->m_extra;
		m_mesh.rotate(rotation);
		break;
	case GA_MSG_PLAY_SOUND: {
		 // Start playing a sound or check if it plays
		alSound* voc = (alSound*)message->m_extra;
		m_mesh.play(voc);
		break;
	}
	case GA_MSG_STOP_SOUND: {
		// Stop playing a sound (or all sound if nullptr)
		alSound* voc = (alSound*)message->m_extra;
		m_mesh.stop(voc);
		break;
	}
	}
}

ComponentMesh::~ComponentMesh()
{
}
