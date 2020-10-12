#include "gaComponentMesh.h"

#include "../../alEngine/alSound.h"

#include "../World.h"
#include "../gaEntity.h"

using namespace GameEngine;

ComponentMesh::ComponentMesh(void):
	gaComponent(gaComponent::MESH)
{
}

ComponentMesh::ComponentMesh(fwGeometry* _geometry, fwMaterial* _material):
	gaComponent(gaComponent::MESH),
	fwMesh(_geometry, _material)
{
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
			translate(position);
		}
		else {
			translate(m_entity->position());
			rotate(m_entity->quaternion());
			set_scale(m_entity->get_scale());
			worldMatrix(m_entity->worldMatrix(),
				m_entity->inverseWorldMatrix()
			);
		}
		break;
	}
	case gaMessage::ROTATE: {
		if (message->m_value == gaMessage::Flag::ROTATE_VEC3) {
			glm::vec3* rotation = (glm::vec3*)message->m_extra;
			rotate(rotation);
		}
		else {
			glm::quat* quaternion = (glm::quat*)message->m_extra;
			rotate(quaternion);
		}
		break;
	}
	case gaMessage::PLAY_SOUND: {
		 // Start playing a sound or check if it plays
		alSound* voc = (alSound*)message->m_extra;
		play(voc);
		break;
	}
	case gaMessage::STOP_SOUND: {
		// Stop playing a sound (or all sound if nullptr)
		alSound* voc = (alSound*)message->m_extra;
		stop(voc);
		break;
	}

	case gaMessage::WORLD_INSERT:
		g_gaWorld.add2scene(this);
		break;

	case gaMessage::WORLD_REMOVE:
		g_gaWorld.remove2scene(this);
		break;
	}
}

ComponentMesh::~ComponentMesh()
{
}
