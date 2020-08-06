#include "gaEntity.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../framework/fwScene.h"
#include "../framework/fwMesh.h"

#include "gaWorld.h"
#include "../gaEngine/gaBoundingBoxes.h"

static int g_ids = 0;

gaEntity::gaEntity(int mclass, const std::string& name) :
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
	m_collider.set(&m_modelAABB, &m_worldMatrix, &m_inverseWorldMatrix);
}

gaEntity::gaEntity(int mclass, const std::string& name, const glm::vec3& position):
	fwObject3D(position),
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
	m_collider.set(&m_modelAABB, &m_worldMatrix, &m_inverseWorldMatrix);
}

/**
 * extend the components of the entity
 */
void gaEntity::addComponent(gaComponent* component)
{
	m_components.push_back(component);
	component->parent(this);
}

/**
 * check all components to find one with the proper type
 */
gaComponent* gaEntity::findComponent(int type)
{
	for (auto component : m_components) {
		if (component->is(type)) {
			return component;
		}
	}

	return nullptr;
}

/**
 * add an entity inside that one (and increase the AABB if needed)
 */
void gaEntity::addChild(gaEntity* entity)
{
	m_children.push_back(entity);
	m_modelAABB.extend(entity->m_modelAABB);
	updateWorldAABB();
}

/**
 * quick AABB check for entities collision
 */
bool gaEntity::collideAABB(fwAABBox& box)
{
	return m_worldBounding.intersect(box);
}

/**
 * extended collision using colliders
 */
bool gaEntity::collide(gaEntity* entity, const glm::vec3 &direction, glm::vec3 &collision)
{
	return m_collider.collision(entity->m_collider, direction, collision);
}

/**
 * extended collision using colliders
 */
bool gaEntity::collide(GameEngine::Collider collider, const glm::vec3& direction, glm::vec3& collision)
{
	return m_collider.collision(collider, direction, collision);
}

/**
 * set the model space AABB
 */
void gaEntity::modelAABB(const fwAABBox& box)
{
	m_modelAABB = box;
}
/**
 * rotate the object and update the AABB
 */
void gaEntity::rotate(const glm::vec3& rotation)
{
	m_quaternion = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));

	// take the opportunity to update the world bounding box
	updateWorldAABB();
}

/**
 * distance between the 2 entities
 */
float gaEntity::distanceTo(gaEntity* other)
{
	return glm::distance(m_position, other->m_position);
}

/**
 * distance from the entity position to the point
 */
float gaEntity::distanceTo(const glm::vec3& p)
{
	return glm::distance(m_position, p);
}

/**
 * Send message to another entity
 */
void gaEntity::sendMessage(const std::string& target, int action, int value, void* extra)
{
	g_gaWorld.sendMessage(m_name, target, action, value, extra);
}

/**
 * Send message to the world
 */
void gaEntity::sendMessageToWorld(int action, int value, void* extra)
{
	g_gaWorld.sendMessage(m_name, "_world", action, value, extra);
}

/**
 * send internal message to all components for immediate action
 */
void gaEntity::sendInternalMessage(int action, int value, void* extra)
{
	gaMessage message("_component", "_component", action, value, extra);
	dispatchMessage(&message);
}

/**
 * Send a delayed message to myself
 */
void gaEntity::sendDelayedMessage(int action, int value, void* extra)
{
	g_gaWorld.sendMessageDelayed(m_name, m_name, action, value, extra);
}

/**
 * if the entity has a mesh, add to the scene
 */
void gaEntity::add2scene(fwScene* scene)
{
	if (m_mesh) {
		m_scene = scene;
		m_scene->addChild(m_mesh);
	}
}

/**
 * Update the object position (given in gl space) and update the worldboundingBox(in gl space)
 */
void gaEntity::moveTo(const glm::vec3& position)
{
	m_position = position;

	sendInternalMessage(GA_MSG_MOVE, 0, &m_position);
}

/**
 * update the world AABB based on position
 */
void gaEntity::updateWorldAABB(void)
{
	updateWorldMatrix(nullptr);

	m_worldBounding.apply(m_modelAABB, m_worldMatrix);
}

/**
 * let an entity deal with a situation
 */
void gaEntity::dispatchMessage(gaMessage* message)
{
	switch (message->m_action)
	{
	case GA_MSG_MOVE_TO:
		// move the entity following a direction
		glm::vec3 direction = *(glm::vec3*)message->m_extra;
		m_position += direction;

		sendInternalMessage(GA_MSG_MOVE, 0, &m_position);
		break;

	case GA_MSG_MOVE:
		m_position = *(glm::vec3*)message->m_extra;

		// take the opportunity to update the world bounding box
		updateWorldAABB();

		// detect if the entity moved to a new sector
		m_supersector = g_gaWorld.findSector(m_supersector, m_position);
		break;

	case GA_MSG_ROTATE:
		// take the opportunity to update the world bounding box
		if (message->m_value == 0) {
			m_rotation = *(glm::vec3*)message->m_extra;
			m_quaternion = glm::quat(glm::vec3(m_rotation.x, m_rotation.y, m_rotation.z));
			updateWorldAABB();
		}
		else {
			m_quaternion = *(glm::quat*)message->m_extra;
			m_worldBounding.transform(m_modelAABB, m_position, m_quaternion, glm::vec3(0.10f));
		}
		break;

	default:
		break;
	}

	for (auto component : m_components) {
		component->dispatchMessage(message);
	}
}

/**
 * extended collision test after a successful AABB collision
 */
bool gaEntity::checkCollision(fwCylinder& bounding, glm::vec3& direction, glm::vec3& intersection, std::list<gaCollisionPoint>& collisions)
{
	// default entities are included in the AABB, so the test is always positive
	return true;
}

gaEntity::~gaEntity()
{
	g_gaWorld.removeClient(this);
	g_gaBoundingBoxes.remove(&m_worldBounding);

	if (m_mesh) {
		if (m_scene) {
			m_scene->removeChild(m_mesh);
		}
		delete m_mesh;
	}
}