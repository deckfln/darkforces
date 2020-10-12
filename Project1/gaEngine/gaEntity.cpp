#include "gaEntity.h"

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../framework/fwScene.h"
#include "../framework/fwMesh.h"

#include "World.h"
#include "../gaEngine/gaBoundingBoxes.h"

static int g_ids = 0;

gaEntity::gaEntity(int mclass, const std::string& name) :
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
	m_collider.set(&m_modelAABB, &m_worldMatrix, &m_inverseWorldMatrix);

	m_transforms.m_flag = gaMessage::Flag::WANT_TO_MOVE_FALL;	// accept falling down
}

gaEntity::gaEntity(int mclass, const std::string& name, const glm::vec3& position):
	fwObject3D(position),
	m_entityID(g_ids++),
	m_name(name),
	m_class(mclass)
{
	g_gaBoundingBoxes.add(&m_worldBounding);
	m_collider.set(&m_modelAABB, &m_worldMatrix, &m_inverseWorldMatrix);

	m_transforms.m_flag = gaMessage::Flag::WANT_TO_MOVE_FALL;	// accept falling down
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
bool gaEntity::collideAABB(const fwAABBox& box)
{
	return m_worldBounding.intersect(box);
}

bool gaEntity::collideAABB(gaEntity const* with)
{
	return m_worldBounding.intersect(with->m_worldBounding);
}

/**
 * extended collision using colliders
 */
bool gaEntity::collide(gaEntity* entity, 
	const glm::vec3& forward,
	const glm::vec3& down,
	std::vector<gaCollisionPoint>& collisions)
{
	return m_collider.collision(entity->m_collider, forward, down, collisions);
}

/**
 * extended collision using colliders
 */
bool gaEntity::collide(const GameEngine::Collider& collider, 
	const glm::vec3& forward,
	const glm::vec3& down,
	std::vector<gaCollisionPoint>& collisions)
{
	return m_collider.collision(collider, forward, down, collisions);
}

/**
 * check if the entity moved so fast it went trough another one
 */
bool gaEntity::warpThrough(const GameEngine::Collider& collider, 
	const glm::vec3& old_position, 
	std::vector<gaCollisionPoint>& collisions)
{
	glm::vec3 center = m_worldBounding.center();
	glm::vec3 d = center - position();
	glm::vec3 old_center = old_position + d;

	return m_collider.warpThrough(collider, center, old_center, collisions);
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
	sendInternalMessage(gaMessage::MOVE, 0, (void *)&position);
}

/**
 * apply a transformation and update the worldAABB
 */
void gaEntity::transform(GameEngine::Transform* transform)
{
	fwObject3D::transform(transform);
	updateWorldAABB();
}

/**
 * maximum radius of the entity
 */
float gaEntity::radius(void)
{
	float r1 = m_modelAABB.m_p1.x - m_modelAABB.m_p.x;
	float r2 = m_modelAABB.m_p1.z - m_modelAABB.m_p.z;
	float r = std::min(r1, r2);

	return r / 2.0f;
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
	case gaMessage::Action::MOVE_TO:
		// move the entity following a direction
		moveBy((glm::vec3*)message->m_extra);

		sendInternalMessage(gaMessage::MOVE, 0, (void *)&position());
		break;

	case gaMessage::Action::MOVE:
		// if the there is no position, this is just a notification
		if (message->m_extra != nullptr) {
			translate((glm::vec3*)message->m_extra);
			updateWorldAABB();
		}

		// detect if the entity moved to a new sector
		m_supersector = g_gaWorld.findSector(m_supersector, position());
		break;

	case gaMessage::Action::ROTATE:
		// take the opportunity to update the world bounding box
		switch (message->m_value) {
		case gaMessage::Flag::ROTATE_VEC3:
				rotate((glm::vec3*)message->m_extra);
				break;
		case gaMessage::Flag::ROTATE_QUAT:
				rotate((glm::quat*)message->m_extra);
				break;
		case gaMessage::Flag::ROTATE_BY:
				rotateBy((glm::vec3*)message->m_extra);
				break;
		}
		updateWorldAABB();
		break;

	case gaMessage::Action::FALL:
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