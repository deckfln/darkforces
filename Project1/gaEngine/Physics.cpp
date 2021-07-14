#include "Physics.h"

#include <memory>
#include <map>
#include <glm/gtx/normal.hpp>

#include "World.h"
#include "gaMessage.h"
#include "gaEntity.h"
#include "gaCollisionPoint.h"
#include "gaDebug.h"
#include "gaActor.h"

#include "../config.h"

#include "../darkforces/dfSuperSector.h"

using namespace GameEngine;

const float EPSILON = 0.001f;

Physics::Physics(World* world) :
	m_world(world)
{
}

/**
 * Test if the entity warped through a triangle
 */
bool Physics::warpThrough(gaEntity *entity, 
	const glm::vec3& old_position,
	Transform& tranform,
	std::vector<gaCollisionPoint>& collisions)
{
	// do a warpThrough quick test
	glm::vec3 warp;
	glm::vec3 center = entity->modelAABB().center();
	fwAABBox aabb_ws(entity->position() + center, old_position + center);
	std::vector<glm::vec3> warps;

	for (auto sector : m_world->m_sectors) {
		if (sector->collideAABB(aabb_ws)) {
			// do a warpTrough full test
			entity->warpThrough(sector->collider(), old_position, collisions);
		}
	}

	if (collisions.size() > 0) {
		float nearest = 9999999;
		glm::vec3 near_c = glm::vec3(0);

		dfSuperSector* collidedEntity = nullptr;

		if (entity->name() == "player")
			gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " warp detected");

		// find the nearest point
		for (auto& collision : collisions) {
			if (collision.m_class == gaCollisionPoint::Source::SECTOR) {
				collidedEntity = static_cast<dfSuperSector*>(collision.m_source);
			}

			if (glm::distance2(old_position, collision.m_position) < nearest) {
				nearest = glm::distance2(old_position, collision.m_position);
				near_c = collision.m_position;
			}
		}
		// and force the object there
		tranform.m_position = near_c;
		// entity->transform(&tranform);
		m_world->sendMessage(
			entity->name(),
			entity->name(),
			gaMessage::Action::COLLIDE,
			0,
			nullptr
		);

		if (entity->name() == "player")
			gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " warp fixed at " + std::to_string(tranform.m_position.x)
				+ " " + std::to_string(tranform.m_position.y)
				+ " " + std::to_string(tranform.m_position.z));

		return true;	// go to next entity
	}

	return false;
}

/**
 * test if the entity collide other entities
 */
void Physics::testEntities(gaEntity* entity, const Transform& tranform, std::vector<gaCollisionPoint>& collisions)
{
	uint32_t size;

	for (auto entry : m_world->m_entities) {
		for (auto ent : entry.second) {
			// ignore ghosts
			if (!ent->hasCollider()) {
				continue;
			}

			if (ent != entity && entity->collideAABB(ent->worldAABB())) {
				size = collisions.size();
				if (entity->collide(ent, tranform.m_forward, tranform.m_downward, collisions)) {
					for (auto i = size; i < collisions.size(); i++) {
						collisions[i].m_source = ent;
						collisions[i].m_class = gaCollisionPoint::Source::ENTITY;
					}
				}
			}
		}
	}
}

/**
 * test if the entity collide sectors
 */
void Physics::testSectors(gaEntity* entity, const Transform& tranform, std::vector<gaCollisionPoint>& collisions)
{
	uint32_t size;
	if (entity->collideSectors()) {
		for (auto sector : m_world->m_sectors) {
			size = collisions.size();
			if (sector->collideAABB(entity->worldAABB()))
			{
				if (entity->collide(sector->collider(), tranform.m_forward, tranform.m_downward, collisions))
				{
					for (auto i = size; i < collisions.size(); i++) {
						collisions[i].m_source = sector;
						collisions[i].m_class = gaCollisionPoint::Source::SECTOR;
					}
				}
			}
		}
	}
}

/**
 * test if the entity collide sectors
 */
bool Physics::testSegmentSector(const glm::vec3& p1, const glm::vec3& p2, fwCollision::Test test)
{
	fwAABBox aabb(p1, p2);

	for (auto sector : m_world->m_sectors) {
		if (sector->collideAABB(aabb)) {
			if (sector->collide(p1, p2, test))
			{
				return true;
			}
		}
	}

	return false;
}

/**
 * display debug stats
 */
static void debugCollision(const gaCollisionPoint& collision, gaEntity* entity, const std::string& msg)
{
	std::string collider;

	if (collision.m_class == gaCollisionPoint::Source::ENTITY) {
		collider = static_cast<gaEntity*>(collision.m_source)->name();
	}
	else {
		collider = static_cast<dfSuperSector*>(collision.m_source)->name();
	}

	gaDebugLog(1, "GameEngine::Physics::wantToMove",
		entity->name() + " found collision " + msg
		+ "with " + collider
		+ " at " + std::to_string(collision.m_position.y)
	);
}

/**
 * Send a collision message between 2 entities
 */
void Physics::informCollision(gaEntity* from, gaEntity* to)
{
	// always inform the source entity 
	m_world->sendMessage(
		from->name(),
		to->name(),
		gaMessage::Action::COLLIDE,
		gaMessage::Flag::COLLIDE_ENTITY,
		nullptr
	);
	// always inform the colliding entity 
	m_world->sendMessage(
		to->name(),
		from->name(),
		gaMessage::Action::COLLIDE,
		gaMessage::Flag::COLLIDE_ENTITY,
		nullptr
	);
}

/**
 * Move a bullet
 */
void Physics::moveBullet(gaEntity* entity, gaMessage* message)
{
	std::vector<gaCollisionPoint> collisions;

	GameEngine::Transform& tranform = entity->transform();
	glm::vec3 old_position = entity->position();

	entity->pushTransformations();
	entity->transform(&tranform);
	glm::vec3 new_position = entity->position();
	glm::vec3 direction = glm::normalize(old_position - new_position);

	// force the worldAABB based by the bullet movement
	entity->worldAABB(new_position, old_position);

	// collide against the entities
	testEntities(entity, tranform, collisions);

	// collide against the sectors
	testSectors(entity, tranform, collisions);

	for (auto& collision : collisions) {
		// and we do not force the move
		// refuse the move and inform both element from the collision
		entity->popTransformations();				// restore previous position

		if (collision.m_class == gaCollisionPoint::Source::ENTITY) {
			m_world->sendMessage(
				static_cast<gaEntity*>(collision.m_source)->name(),
				entity->name(),
				gaMessage::Action::COLLIDE,
				gaMessage::Flag::COLLIDE_ENTITY,
				nullptr
			);
			return;
		}
		else {
			m_world->sendMessage(
				static_cast<dfSuperSector*>(collision.m_source)->name(),
				entity->name(),
				gaMessage::Action::COLLIDE,
				gaMessage::Flag::COLLIDE_WALL,
				nullptr
			);
			return;
		}
	}

	// acknowledge the move
	m_world->sendMessage(
		entity->name(),
		entity->name(),
		gaMessage::Action::MOVE,
		0,
		nullptr
	);
}

/**
 * An entity wants to move
 */
void Physics::moveEntity(gaEntity* entity, gaMessage* message)
{
	if (entity->name() == "post_elev_e") {
		printf("Physics::moveEntity post_elev_e\n");
	}
	// bullets are special case
	if (entity->is(DF_ENTITY_BULLET)) {
		moveBullet(entity, message);
		return;
	}

	std::vector<gaCollisionPoint> collisions;
	std::map<std::string, bool> collidedEntities;

	struct Action {
		gaEntity* m_entity;
		int m_flag;

		Action(gaEntity* entity, int flag) :
			m_entity(entity),
			m_flag(flag) {};
	};

	std::queue<Action> actions;
	std::map<std::string, bool> pushedEntities;	// list of entities we are pushing, only push an entity once per run

	// kick start actions
	actions.push(
		Action(entity, entity->defaultCollision())
	);

	while (actions.size() != 0) {
		collisions.clear();

		Action action = actions.front();
		actions.pop();

		entity = action.m_entity;
		GameEngine::Transform& tranform = entity->transform();
		glm::vec3 old_position = entity->position();

		entity->pushTransformations();
		entity->transform(&tranform);
		glm::vec3 new_position = entity->position();
		glm::vec3 direction = glm::normalize(old_position - new_position);

		if (entity->name() == "player")
			gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " to " + std::to_string(tranform.m_position.x)
				+ " " + std::to_string(tranform.m_position.y)
				+ " " + std::to_string(tranform.m_position.z));

		// check if we warp through a triangle
		if (entity->collideSectors()) {
			if (warpThrough(entity, old_position, tranform, collisions)) {

				// if the entity was driving by physics, remove from the list
				if (m_ballistics.count(entity->name()) > 0) {
					m_remove.push_back(entity->name());
				}

				continue;	// object warped through a triangle
			}
		}

		// check if we warp through an entity AABB
		/*
		float tnear, tfar;
		for (auto& entities : m_world->m_entities) {
			for (auto ent : entities.second) {
				if (ent->hasCollider()) {
					if (Framework::intersectRayAABox2(old_position, new_position, ent->worldAABB(), tnear, tfar)) {
						if (tnear > 0.0f && tfar < 1.0f) {
							printf("Physics::moveEntity\n");
						}
					}
				}
			}
		}
		*/

		// collide against the entities
		testEntities(entity, tranform, collisions);

		// collide against the sectors
		testSectors(entity, tranform, collisions);

		// find the nearest collisions
		gaCollisionPoint* nearest_collision = nullptr;
		gaCollisionPoint* nearest_ground = nullptr;
		gaCollisionPoint* nearest_ceiling = nullptr;		float nearest_ceiling_y = 999999;

		float distance = 99999999.0f;
		float ground = 9999999.0f;
		float d;
		float dBottom = 0;						// distance from worldAAB bottom to collision Y
		float dTop = 0;							// distance from worldAAB top to collision Y
		float lifted = 0;						// if collision on top for elevator, record the Y lifting
		glm::vec3 pushed_aside = glm::vec3(0);	// if collision on border of elevator, record the XZ direction
		bool fall = false;						// are we falling
		bool fix_y = false;						// do we enter the ground and need Y to be fixed

		std::map<std::string, gaEntity*>& sittingOnTop = entity->sittingOnTop();

		gaEntity* collidedEntity;
		pushedEntities.clear();

		for (auto& collision : collisions) {
			collidedEntity = nullptr;
			if (collision.m_class == gaCollisionPoint::Source::ENTITY) {
				collidedEntity = static_cast<gaEntity*>(collision.m_source);
			}

			switch (collision.m_location) {
			case fwCollisionLocation::FRONT:
				d = entity->distanceTo(collision.m_position);
				if (d < distance) {
					nearest_collision = &collision;
					distance = d;
				}
				fall = false;
				break;

			case fwCollisionLocation::BOTTOM:
				fall = false;
				if (entity->gravity()) {
					if (collision.m_position.y < ground) {
						nearest_ground = &collision;
						ground = collision.m_position.y;
					}
				}
				break;

			case fwCollisionLocation::COLLIDE: {

				// non physical entities need no specific handling
				// just trigger a collision
				if (collidedEntity) {
					if (!collidedEntity->physical()) {
						// only send once
						if (collidedEntities.count(collidedEntity->name()) > 0) {
							continue;
						}

						// always inform the source entity 
						collidedEntities[collidedEntity->name()] = true;

						informCollision(collidedEntity, entity);
						continue;	// check next collision
					}
				}

				// test the collided triangle (wall, floor, ceiling)
				const fwAABBox& worldAABB = entity->worldAABB();
				bool isFloor = false;
				bool isCelling = false;
				bool isFacing = true;
				float facing = false;

				if (collision.m_triangle != nullptr) {
					// check if the triangle is horizontal or vertical
					glm::vec3 normal = glm::triangleNormal(
						collision.m_triangle[0],
						collision.m_triangle[1],
						collision.m_triangle[2]
					);
					float horizontal = glm::dot(glm::vec3(0, 1, 0), normal);
					isFloor = horizontal > 0.99;
					isCelling = horizontal < -0.99;

					// check if the triangle is facing the entity
					facing = glm::dot(direction, normal);
					isFacing = facing > 0.0f;
				}

				dTop = worldAABB.m_p1.y - collision.m_position.y;

				if (isFloor) {				// BOTTOM
					float deltaY = new_position.y - collision.m_position.y;

					// if the floor is ABOVE the entity
					if (deltaY < -0.01f) {
						// if the collision point is NOT more or less straight above the entity
						if (abs(new_position.x - collision.m_position.x) > EPSILON || abs(new_position.z - collision.m_position.z) > EPSILON) {
							// test explicitly there is a triangle above the entity
							glm::vec3 bottom2collision(new_position.x, collision.m_position.y + EPSILON, new_position.z);
							if (!testSegmentSector(entity->position(), bottom2collision, fwCollision::Test::WITHOUT_BORDERS)) {
								// OK, so we are colliding with a floor that is not exactly above the entity
								// discard the ground testing
								// convert to a wall testing
								continue;
							}
						}
					}

					// get the nearest to the axe (this is where the entity is sitting on)
					float distance2axe = glm::distance2(
						glm::vec2(new_position.x, new_position.z),
						glm::vec2(collision.m_position.x, collision.m_position.z)
					);
					if (distance2axe < ground) {
						nearest_ground = &collision;
						ground = distance2axe;
					}

					dBottom = collision.m_position.y - worldAABB.m_p.y;

					if (collidedEntity) {
						if (abs(dBottom) > EPSILON) {
							if (entity->name() == "player")
								debugCollision(collision, entity, "on bottom");

							// for pushing objects (like elevators)
							// push the other entity (but do it only once
							if (action.m_flag == gaMessage::Flag::PUSH_ENTITIES && pushedEntities.count(collidedEntity->name()) == 0)
							{
								pushedEntities[collidedEntity->name()] = true;

								GameEngine::Transform& t = collidedEntity->transform();
								t.m_position = collidedEntity->position();
								t.m_position.y += (tranform.m_position.y - old_position.y);

								actions.push(
									Action(collidedEntity, gaMessage::Flag::PUSH_ENTITIES)
								);
							}
						}

						fwAABBox& entityAABB = (fwAABBox&)entity->worldAABB();
						const fwAABBox& colliderAABB = collidedEntity->worldAABB();
						if (entityAABB.isAbove(colliderAABB)) {
							std::map<std::string, gaEntity*>& _sittingOnTop = collidedEntity->sittingOnTop();

							if (_sittingOnTop.count(entity->name()) == 0) {
								_sittingOnTop[entity->name()] = entity;
							}
						}
						else {
							if (sittingOnTop.count(collidedEntity->name()) == 0) {
								sittingOnTop[collidedEntity->name()] = collidedEntity;
							}
						}
					}
				}
				else if (isCelling) {			// TOP
					if (entity->name() == "player")
						debugCollision(collision, entity, " hit ceiling");

					if (collision.m_position.y < nearest_ceiling_y) {
						nearest_ceiling = &collision;
						nearest_ceiling_y = collision.m_position.y;
					}
				}
				else {							// FRONT/BACK/LEFT/RIGHT
					float deltaY = collision.m_position.y - new_position.y;

					// if we are just hitting the bottom of the entity, we are probably hitting the edge of a vertical triangle
					// so discard
					if (abs(deltaY) < 0.01f) {
						continue;
					}

					// if we are hitting a low corner of the cylinder, we might be able to walk over
					// so discard the collision
					if (entity->canStep()) {
						if(deltaY > 0 && deltaY < static_cast<gaActor*>(entity)->step()) {
							continue;
						}
					}

					if (entity->name() == "player")
						debugCollision(collision, entity, " on edge " + std::to_string(facing) + " ");

					d = entity->distanceTo(collision.m_position);
					if (d < distance) {
						nearest_collision = &collision;
						distance = d;
					}

					glm::vec3 p(tranform.m_position.x, 0, tranform.m_position.z),
						p1(collision.m_position.x, 0, collision.m_position.z);

					pushed_aside = glm::normalize(p - p1) * entity->radius();
				}
			}
			}
		}

		// take actions

		if (nearest_collision) {
			// if there is a collision

			if (entity->name() == "player")
				gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " pushes ");

			if (action.m_flag == gaMessage::Flag::PUSH_ENTITIES) {
				// inform being lifted or pushed aside
				if (pushed_aside != glm::vec3(0)) {
					if (nearest_collision->m_class == gaCollisionPoint::Source::ENTITY) {
						// push another entity aside
						gaEntity* pushed = static_cast<gaEntity*>(nearest_collision->m_source);
						actions.push(
							Action(pushed, gaMessage::Flag::PUSH_ENTITIES)
						);

						if (entity->name() == "player")
							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " pushes " + pushed->name());

						GameEngine::Transform& t = pushed->transform();
						t.m_position = entity->position();
						t.m_position += pushed_aside;
					}
					else {
						// being pushed by a sector
						tranform.m_position.x += pushed_aside.x;
						tranform.m_position.z += pushed_aside.z;

						if (m_ballistics.count(entity->name()) > 0 && m_ballistics[entity->name()].m_inUse) {
							m_ballistics[entity->name()].reset(tranform.m_position, new_position);
						}

						actions.push(
							Action(entity, gaMessage::Flag::PUSH_ENTITIES)
						);

						if (entity->name() == "player")
							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " pushed by sector");

						continue; // ignore the floor, will be extracted on next run
					}
				}
			}

			if (action.m_flag != gaMessage::Flag::PUSH_ENTITIES) {

				// if the collision is 'behind' the entity, push the entity forward
				glm::vec3 coll = nearest_collision->m_position - new_position;
				float dot = glm::dot(glm::normalize(coll), direction);

				if (dot > 0 && nearest_collision->m_class == gaCollisionPoint::Source::SECTOR) {
					// being pushed by a sector

					tranform.m_position -= coll;

					if (m_ballistics.count(entity->name()) > 0 && m_ballistics[entity->name()].m_inUse) {
						m_ballistics[entity->name()].reset(tranform.m_position, new_position);
					}

					actions.push(
						Action(entity, gaMessage::Flag::PUSH_ENTITIES)
					);

					if (entity->name() == "player")
						gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " pushed by sector");

					continue; // ignore the floor, will be extracted on next run
				}
				else {
					// and we do not force the move
					// refuse the move and inform both element from the collision

					if (nearest_collision->m_class == gaCollisionPoint::Source::ENTITY) {
						gaEntity* collisionWith = static_cast<gaEntity*>(nearest_collision->m_source);

						// ONLY refuse the move if the entity is a physical one
						if (collisionWith->physical()) {
							if (entity->name() == "player")
								gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " deny move " + std::to_string(tranform.m_position.x)
									+ " " + std::to_string(tranform.m_position.y)
									+ " " + std::to_string(tranform.m_position.z));

							entity->popTransformations();				// restore previous position
						}
						else {
							if (entity->name() == "player")
								gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " collide " + std::to_string(tranform.m_position.x)
									+ " " + std::to_string(tranform.m_position.y)
									+ " " + std::to_string(tranform.m_position.z));
						}

						// always inform the source entity 
						informCollision(collisionWith, entity);
					}
					else {
						// sectors always block the movement
						entity->popTransformations();				// restore previous position

						m_world->sendMessage(
							static_cast<dfSuperSector*>(nearest_collision->m_source)->name(),
							entity->name(),
							gaMessage::Action::COLLIDE,
							gaMessage::Flag::COLLIDE_WALL,
							nullptr
						);
					}
					continue;									// block the move
				}
			}
		}

		// only test ceiling collision if the entity is moving up
		if (nearest_ceiling != nullptr && new_position.y > old_position.y) {
			// if the entity hit the ceiling BUT can be pushed aside by another triangle
			if (nearest_ceiling->m_class == gaCollisionPoint::Source::ENTITY) {
				gaEntity* collisionWith = static_cast<gaEntity*>(nearest_ceiling->m_source);

				// ONLY refuse the move if the entity is a physical one
				if (collisionWith->physical()) {
					if (entity->name() == "player")
						gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " deny move upward " + std::to_string(tranform.m_position.x)
							+ " " + std::to_string(tranform.m_position.y)
							+ " " + std::to_string(tranform.m_position.z));

					entity->popTransformations();				// restore previous position
				}
				else {
					if (entity->name() == "player")
						gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " collide upward " + std::to_string(tranform.m_position.x)
							+ " " + std::to_string(tranform.m_position.y)
							+ " " + std::to_string(tranform.m_position.z));
				}

				// always inform the source entity 
				informCollision(collisionWith, entity);
			}
			else {
				// sectors always block the movement
				entity->popTransformations();				// restore previous position

				// stop ballistic move
				if (m_ballistics.count(entity->name()) > 0 && m_ballistics[entity->name()].m_inUse) {
					// if the object was going upward, force the position
					tranform.m_position.y = nearest_ceiling_y;
					m_ballistics[entity->name()] = Ballistic(tranform.m_position, old_position);

					actions.push(
						Action(entity, gaMessage::Flag::PUSH_ENTITIES)
					);	// fix the entity altitude
					fix_y = true;
					if (entity->name() == "player")
						gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " falling to ground " + std::to_string(tranform.m_position.x)
							+ " " + std::to_string(tranform.m_position.y)
							+ " " + std::to_string(tranform.m_position.z));
				}

				if (entity->name() == "player")
					gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " hit sector ceiling " + std::to_string(tranform.m_position.x)
					+ " " + std::to_string(tranform.m_position.y)
					+ " " + std::to_string(tranform.m_position.z));

				m_world->sendMessage(
					static_cast<dfSuperSector*>(nearest_ceiling->m_source)->name(),
					entity->name(),
					gaMessage::Action::COLLIDE,
					gaMessage::Flag::COLLIDE_WALL,
					nullptr
				);
			}
			continue;									// block the move
		}

		// manage ground collision and accept to jump up if over a step
		if (entity->gravity()) {
			if (nearest_ground) {
				if (m_ballistics.count(entity->name()) > 0 && m_ballistics[entity->name()].m_inUse) {
					// if the object was falling, remove from the list and force the position
					m_remove.push_back(entity->name());
					m_ballistics[entity->name()].m_inUse = false;

					tranform.m_position.y = nearest_ground->m_triangle[0].y;
					fix_y = true;
					if (entity->name() == "player")
						gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " falling to ground " + std::to_string(tranform.m_position.x)
							+ " " + std::to_string(tranform.m_position.y)
							+ " " + std::to_string(tranform.m_position.z));
				}
				else if (entity->canStep()) {
					if (new_position.y - nearest_ground->m_position.y < static_cast<gaActor*>(entity)->step()) {
						// if there is a step and the entity can step over
						if (abs(nearest_ground->m_position.y - new_position.y) > EPSILON) {
							// if more than epsilon, fix the position
							tranform.m_position.y = nearest_ground->m_position.y;
							actions.push(
								Action(entity, gaMessage::Flag::PUSH_ENTITIES)
							);	// fix the entity altitude
							fix_y = true;
							if (entity->name() == "player")
  								gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " fixed ground " + std::to_string(tranform.m_position.x)
									+ " " + std::to_string(tranform.m_position.y)
									+ " " + std::to_string(tranform.m_position.z));
						}
					}
				}
				else {
					// convert the down collision to a nearest
					nearest_collision = nearest_ground;
				}
			}
			else {
				// if there is no floor

				// check if there is a at STEP distance below the entity
				if (entity->canStep()) {
					float step = static_cast<gaActor*>(entity)->step();
	 				glm::vec3 down(new_position.x, new_position.y - step, new_position.z);
					if (testSegmentSector(entity->position(), down, fwCollision::Test::WITHOUT_BORDERS)) {
						// we found a floor !
						tranform.m_position.y -= step;
						actions.push(
							Action(entity, gaMessage::Flag::PUSH_ENTITIES)
						);	
						
						// fix the entity altitude and give it another try
						if (entity->name() == "player")
 							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " step down to ground " + std::to_string(tranform.m_position.x)
								+ " " + std::to_string(tranform.m_position.y)
								+ " " + std::to_string(tranform.m_position.z));

						continue;
					}

				}

				// OK we definitely need to fall
				switch (tranform.m_flag) {
				case gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL:
					// if the entity wants to be informed of falling
					entity->popTransformations();			// restore previous position
					m_world->sendMessage(
						entity->name(),
						entity->name(),
						gaMessage::Action::WOULD_FALL,
						0,
						nullptr
					);
					continue;
				default:
					if (m_ballistics.count(entity->name()) == 0) {
						// if the entity wants to be informed of falling
						m_world->sendMessage(
							entity->name(),
							entity->name(),
							gaMessage::Action::FALL,
							0,
							nullptr
						);

						// engage ballistic
						m_ballistics[entity->name()] = Ballistic(tranform.m_position, old_position);
					}

					if (entity->name() == "player")
						gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " falling");
					continue;
				}
			}
		}

		// accept the move if we do not fix the position
		if (!fix_y) {
			m_world->sendMessage(
				entity->name(),
				entity->name(),
				gaMessage::Action::MOVE,
				0,
				nullptr
			);
		}

		// inform all objects that are sitting on that one
		lifted = tranform.m_position.y - old_position.y;
		if (lifted < 0) {
			for (auto entry : sittingOnTop) {
				gaEntity* pushed = entry.second;
				actions.push(
					Action(pushed, gaMessage::Flag::PUSH_ENTITIES)
				);

				GameEngine::Transform& t = pushed->transform();
				t.m_position = pushed->position();
				t.m_position.y += lifted;
			}
			sittingOnTop.clear();	// clear the cache, it will be reimplemented by the object if it is still on top
		}
	}
}

/**
 * Deal with falling objects
 */
void Physics::update(time_t delta)
{
	m_remove.clear();

	for (auto& en : m_ballistics) {
		Ballistic& b = en.second;

		for (auto entity : m_world->m_entities[en.first]) {
			gaDebugLog(1, "GameEngine::Physics::update", entity->name());
			b.apply(delta, entity->pTransform());

			moveEntity(entity, nullptr);
		}
	}

	for (auto& name : m_remove) {
		m_ballistics.erase(name);
	}
}

