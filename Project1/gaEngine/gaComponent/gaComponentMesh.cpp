#include "gaComponentMesh.h"

#include "../../alEngine/alSound.h"

#include "../gaWorld.h"
#include "../gaEntity.h"

using namespace GameEngine;

ComponentMesh::ComponentMesh(void):
	gaComponent(GA_COMPONENT_MESH)
{
}

ComponentMesh::ComponentMesh(fwGeometry* _geometry, fwMaterial* _material):
	gaComponent(GA_COMPONENT_MESH),
	fwMesh(_geometry, _material)
{
}

/**
 * handle fwMesh actions
 */
void ComponentMesh::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case GA_MSG_MOVE: {
		if (message->m_extra != nullptr) {
			glm::vec3* position = (glm::vec3*)message->m_extra;
			translate(position);
		}
		else {
			worldMatrix(m_entity->worldMatrix(),
				m_entity->inverseWorldMatrix()
			);
		}
		break;
	}
	case GA_MSG_ROTATE: {
		if (message->m_value == 0) {
			glm::vec3* rotation = (glm::vec3*)message->m_extra;
			rotate(rotation);
		}
		else {
			glm::quat* quaternion = (glm::quat*)message->m_extra;
			rotate(quaternion);
		}
		break;
	}
	case GA_MSG_PLAY_SOUND: {
		 // Start playing a sound or check if it plays
		alSound* voc = (alSound*)message->m_extra;
		play(voc);
		break;
	}
	case GA_MSG_STOP_SOUND: {
		// Stop playing a sound (or all sound if nullptr)
		alSound* voc = (alSound*)message->m_extra;
		stop(voc);
		break;
	}

	case GA_MSG_WORLD_INSERT:
		g_gaWorld.add2scene(this);
		break;

	case GA_MSG_WORLD_REMOVE:
		g_gaWorld.remove2scene(this);
		break;
	}
}

ComponentMesh::~ComponentMesh()
{
}
