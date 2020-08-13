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
bool gaEntity::collide(gaEntity* entity, 
	const glm::mat4& worldMatrix,
	const glm::vec3& forward,
	const glm::vec3& down,
	std::list<gaCollisionPoint>& collisions)
{
	return m_collider.collision(entity->m_collider, worldMatrix, forward, down, collisions);
}

/**
 * extended collision using colliders
 */
bool gaEntity::collide(GameEngine::Collider collider, 
	const glm::mat4& worldMatrix,
	const glm::vec3& forward,
	const glm::vec3& down,
	std::list<gaCollisionPoint>& collisions)
{
	return m_collider.collision(collider, worldMatrix, forward, down, collisions);
}

/**
 * set the model space AABB
 */
void gaEntity::modelAABB(const fwAABBox& box)
{
	m_modelAABB = box;
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

void gaEntity::sendDelayedMessageToWorld(int action, int value, void* extra)
{
	g_gaWorld.sendMessageDelayed(m_name, "_world", action, value, extra);
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
	sendInternalMessage(GA_MSG_MOVE, 0, (void *)&position);
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
 * try to move an manage collision
 */
int gaEntity::tryToMove(int flag, const glm::mat4& worldMatrix, const glm::vec3& direction)
{
	switch (g_gaWorld.wantToMove(this, flag, worldMatrix, direction)) {
	case GA_MSG_COLLIDE:
		return GA_MSG_COLLIDE;

	case GA_MSG_WOULD_FALL:
		return GA_MSG_WOULD_FALL;
	}

	return GA_MSG_MOVE;
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
		moveBy((glm::vec3*)message->m_extra);

		sendInternalMessage(GA_MSG_MOVE, 0, (void *)&position());
		break;

	case GA_MSG_MOVE:
		translate((glm::vec3*)message->m_extra);
		updateWorldAABB();
		// detect if the entity moved to a new sector
		m_supersector = g_gaWorld.findSector(m_supersector, position());
		break;

	case GA_MSG_ROTATE:
		// take the opportunity to update the world bounding box
		switch (message->m_value) {
			case GA_MSG_ROTATE_VEC3:
				rotate((glm::vec3*)message->m_extra);
				break;
			case GA_MSG_ROTATE_QUAT:
				rotate((glm::quat*)message->m_extra);
				break;
			case GA_MSG_ROTATE_BY:
				rotateBy((glm::vec3*)message->m_extra);
				break;
		}
		updateWorldAABB();
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