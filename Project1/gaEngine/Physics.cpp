#include "Physics.h"

#include <memory>
#include <map>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>

#include "World.h"
#include "gaMessage.h"
#include "gaEntity.h"
#include "gaCollisionPoint.h"
#include "gaDebug.h"
#include "gaActor.h"

#include "../config.h"

#include "../flightRecorder/Ballistic.h"

#include "../darkforces/dfConfig.h"
#include "../darkforces/dfSuperSector.h"

using namespace GameEngine;

Physics g_gaPhysics;							// physic/collision engine

const float EPSILON = 0.001f;

Physics::Physics(void)
{
}

/**
 *
 Test if the entity warped through a triangle
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

	for (auto sector : g_gaWorld.m_sectors) {
		if (sector->collideAABB(aabb_ws)) {
			// do a warpTrough full test
			entity->warpThrough(sector->collider(), old_position, collisions);
		}
	}

	if (collisions.size() > 0) {
		/*
		if (entity->name() == "MOUSEBOT.3DO(157)") {
			__debugbreak();
		}
		*/

		float nearest = 9999999;
		glm::vec3 near_c = glm::vec3(0);
		gaEntity* collided=nullptr;

		if (entity->name() == "player")
			gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " warp detected");

		// find the nearest point
		for (auto& collision : collisions) {
			if (glm::distance2(old_position, collision.m_position) < nearest) {
				nearest = glm::distance2(old_position, collision.m_position);
				near_c = collision.m_position;
				collided = static_cast<gaEntity*>(collision.m_source);
			}
		}

		// and force the object there
		if (old_position.y != tranform.m_position.y) {
			// vertical warp through
			entity->translate(near_c);
		}
		else {
			// block move and move to previous position
			entity->popTransformations();

			// or move away from the bad position
//			glm::vec3 d = glm::normalize(near_c - old_position);
//			d.y = 0;
//			tranform.m_position += d * entity->radius();
//			
		}

		// always inform the source entity
		informCollision(collided, entity, near_c);

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

	for (auto& entry : g_gaWorld.m_entities) {
		for (auto ent : entry.second) {
			// ignore ghosts
			if (!ent->hasCollider()) {
				continue;
			}

			if (ent->name() != entity->name()) {
				size = collisions.size();
				if (entity->collide(ent, tranform.m_forward, tranform.m_downward, collisions)) {
					for (auto i = size; i < collisions.size(); i++) {
						collisions[i].m_source = ent;
					}
				}
			}
		}
	}
}

/**
 * test if the entity collide sectors
 */
float Physics::ifCollideWithSectorOrEntity(const glm::vec3& p1, const glm::vec3& p2, fwCollision::Test test, gaEntity * entity)
{
	static glm::mat4 worldMatrix(1.0);
	static glm::mat4 inverseWorldMatrix = glm::inverse(worldMatrix);
	static fwAABBox aabb;
	static Collider c;
	static Framework::Segment s;

	aabb.set(p1, p2);
	s.set(p1, p2);
	c.set(&s, & worldMatrix, & inverseWorldMatrix, & aabb);

	glm::vec3 p;
	fwAABBox::Intersection r;

	// test again entities
	for (auto& entry : g_gaWorld.m_entities) {
		for (auto ent : entry.second) {
			// ignore ghosts and itself
			if (!ent->hasCollider() || ent == entity) {
				continue;
			}
			r = ent->intersect(c, p);
			if (r == fwAABBox::Intersection::INTERSECT) {
				return p.y;
			}
		}
	}

	return INFINITY;
}

/**
 * display debug stats
 */
static void debugCollision(const gaCollisionPoint& collision, gaEntity* entity, const std::string& msg)
{
	std::string collider;

	collider = static_cast<gaEntity*>(collision.m_source)->name();

	gaDebugLog(1, "GameEngine::Physics::wantToMove",
		entity->name() + " found collision " + msg
		+ " with " + collider
		+ " at " + std::to_string(collision.m_position.y)
	);
}

/**
 * Send a collision message between 2 entities
 */
void Physics::informCollision(gaEntity* from, gaEntity* to, const glm::vec3& collision)
{
	// always inform the source entity 
	g_gaWorld.sendMessage(
		from->name(),
		to->name(),
		gaMessage::Action::COLLIDE,
		collision,
		nullptr
	);
	// always inform the colliding entity 
	g_gaWorld.sendMessage(
		to->name(),
		from->name(),
		gaMessage::Action::COLLIDE,
		collision,
		nullptr
	);
}

/**
 * Move a bullet
 */
void Physics::moveBullet(gaEntity* entity, gaMessage* message)
{
	gaEntity* collidedEntity = nullptr;
	glm::vec3 collision;
	float min_len = +INFINITY;

	GameEngine::Transform& tranform = entity->transform();
	glm::vec3 old_position = entity->position();

	entity->pushTransformations();
	entity->transform(&tranform);
	glm::vec3 new_position = entity->position();

	gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " to " + std::to_string(tranform.m_position.x)
		+ " " + std::to_string(tranform.m_position.y)
		+ " " + std::to_string(tranform.m_position.z));

	Framework::Segment s(old_position, new_position);
	fwAABBox aabb(s);
	glm::mat4 worldMatrix(1);
	glm::mat4 inverse = glm::inverse(worldMatrix);
	GameEngine::Collider collider(&s, &worldMatrix, &inverse, &aabb);
	glm::vec3 p;

	glm::vec3 forward(0), down(0);
	std::vector<gaCollisionPoint> collisions;

	// test again entities
	float len;

	for (auto& entry : g_gaWorld.m_entities) {
		for (auto ent : entry.second) {
			// ignore ghosts and itself
			if (!ent->physical() || ent == entity) {
				continue;
			}

			// quick tests 
			if (!aabb.intersect(ent->worldAABB())) {
				continue;
			}

			if (ent->worldAABB().intersect(s, p) == fwAABBox::Intersection::NONE) {
				continue;
			}

			// extended test
			collisions.clear();
			if (ent->collide(collider, forward, down, collisions)) {
				for (auto& c : collisions) {
					len = glm::distance2(old_position, c.m_position);
					if (len < min_len) {
						collision = c.m_position;
						min_len = len;
						collidedEntity = ent;
					}
				}
			}
		}
	}

	if (collidedEntity != nullptr) {
		// and we do not force the move
		// refuse the move and inform both element from the collision
		entity->popTransformations();				// restore previous position

		g_gaWorld.sendMessage(
			collidedEntity->name(),
			entity->name(),
			gaMessage::Action::COLLIDE,
			0,
			nullptr
		);
		return;
	}

	// acknowledge the move
	g_gaWorld.sendMessage(
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
	// bullets are special case
	if (message->m_value == gaMessage::Flag::WANT_TO_MOVE_LASER) {
		moveBullet(entity, message);
		return;
	}

	std::vector<gaCollisionPoint> collisions;
	std::map<std::string, bool> collidedEntities;

	std::queue<gaEntity *> actions;
	std::map<std::string, bool> pushedEntities;	// list of entities we are pushing, only push an entity once per run

	// kick start actions
	actions.push(entity);

	bool first = true;
	while (actions.size() != 0) {
		collisions.clear();

		entity = actions.front();
		actions.pop();

		GameEngine::Transform& tranform = entity->transform();

		glm::vec3 old_position = entity->position();

		/*
		static bool first = true;
		if (entity->name() == "OFFCFIN.WAX(21)" && first) {
			first = false;
			old_position = glm::vec3(-22.500357, 0.400000, 29.281124);
			tranform.m_position = glm::vec3(-22.521852, 0.400000, 29.292812);
			g_gaWorld.m_entities["elev3-5"].front()->translate(glm::vec3(-28.0000057, 0.178759009, 29.2000008));
			g_gaWorld.m_entities["elev3-5"].front()->updateWorldAABB();
			g_gaWorld.m_entities["elev3-5"].front()->physical(true);
		}
		*/

		if (entity->name() == "player") {
			gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " to " + std::to_string(tranform.m_position.x)
				+ " " + std::to_string(tranform.m_position.y)
				+ " " + std::to_string(tranform.m_position.z));
		}

		entity->pushTransformations();
		entity->transform(&tranform);
		glm::vec3 new_position = entity->position();
		glm::vec3 direction = glm::normalize(old_position - new_position);

		// check if we warp through a triangle
			/*
		if (entity->collideSectors()) {
			if (warpThrough(entity, old_position, tranform, collisions)) {
				// if the entity was driving by physics, remove from the list
				if (m_ballistics.count(entity->name()) > 0) {
					m_remove.push_back(entity->name());
				}

				continue;	// object warped through a triangle
			}
		}
		*/

		// non collider object do not trigger collisions
		// collide against the entities
		if (entity->hasCollider()) {
			testEntities(entity, tranform, collisions);
		}

		/*
		if (collisions.size() == 0) {
			gaEntity* ent = g_gaWorld.m_entities["elev3-5"].front();

				if (entity->collide(ent, tranform.m_forward, tranform.m_downward, collisions)) {
					__debugbreak();
				}

		}
		*/

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
		bool block_move = false;				// we found a collision that needs to block the request

		std::map<std::string, gaEntity*>& sittingOnTop = entity->sittingOnTop();

		gaEntity* collidedEntity;

		for (auto& collision : collisions) {
			collidedEntity = static_cast<gaEntity*>(collision.m_source);
		
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

						informCollision(collidedEntity, entity, collision.m_position);

						// accept the move at that stage
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
							if (ifCollideWithSectorOrEntity(entity->position(), bottom2collision, fwCollision::Test::WITHOUT_BORDERS, entity) == INFINITY) {
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
							// push the other entity (but do it only once)
							if (collidedEntity->movable() &&
								pushedEntities.count(collidedEntity->name()) == 0)
							{
								pushedEntities[collidedEntity->name()] = true;

								GameEngine::Transform& t = collidedEntity->transform();
								t.m_position = collidedEntity->position();
								t.m_position.y = tranform.m_position.y; // += (tranform.m_position.y - old_position.y);

								actions.push(collidedEntity);
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
				}
			}
			}
		}

		// take actions
		if (nearest_collision) {
			// if there is a collision
			gaEntity* pushed = static_cast<gaEntity*>(nearest_collision->m_source);

			if (entity->name() == "player")
				gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " trying to pushe");

			if (!entity->movable() && pushed->movable()) {

				// the entity (non movable) pushes the collided (movable)
				actions.push(pushed);

				if (entity->name() == "player")
					gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " pushes " + pushed->name());

				GameEngine::Transform& t = pushed->transform();
				glm::vec3 p(new_position.x, 0, new_position.z),
						  p1(nearest_collision->m_position.x, 0, nearest_collision->m_position.z);

				pushed_aside = p1 - p;
				t.m_position = pushed->position();
				t.m_position += pushed_aside;
			}
			else if (entity->movable() && !pushed->movable()) {

				// the entity (movable) is being pushed by collided (non movable)
				float radius = entity->radius();

				glm::vec2 p_collision(nearest_collision->m_position.x, nearest_collision->m_position.z);
				glm::vec2 p_old(old_position.x, old_position.z);
				glm::vec2 delta = glm::normalize(p_collision - p_old) * (radius + EPSILON);

				glm::vec3 p1(nearest_collision->m_position.x, new_position.y, nearest_collision->m_position.z);

				// push by an average radius
				pushed_aside = glm::normalize(new_position - old_position) ;

				tranform.m_position = p1 - glm::vec3(delta.x, 0, delta.y);

				float dot = glm::dot(direction, pushed_aside);

				// if the moved back position is nearly the same as the original position, block the movement
				if (dot > 0 ) {
					// ONLY refuse the move if the entity is a physical one
					if (pushed->physical()) {
						if (entity->name() == "player")
  							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " deny move " + std::to_string(tranform.m_position.x)
								+ " " + std::to_string(tranform.m_position.y)
								+ " " + std::to_string(tranform.m_position.z));

						entity->popTransformations();				// restore previous position
						block_move = true;
					}
					else {
						if (entity->name() == "player")
							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " collide " + std::to_string(tranform.m_position.x)
								+ " " + std::to_string(tranform.m_position.y)
								+ " " + std::to_string(tranform.m_position.z));
					}

					// always inform the source entity
					informCollision(pushed, entity, nearest_collision->m_position);
				}
				else {
					const std::string& name = entity->name();

					if (m_ballistics.count(name) > 0 && m_ballistics[name].m_inUse) {
						glm::vec3 v0(0, new_position.y - old_position.y, 0);

						m_ballistics[name].reset(tranform.m_position, v0);
					}

					if (glm::length(tranform.m_position - old_position) > EPSILON) {
						entity->transform(&tranform);

						if (name == "player")
							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " pushed by sector");
					}
				}
			}
			else if (entity->movable() && pushed->movable()) {
				// both objects can be pushed, so "share" the delta

				glm::vec3 center((nearest_collision->m_position + new_position)/2.0f);

				// push the collided a little bit
				GameEngine::Transform& t = pushed->transform();
				glm::vec3 p(new_position.x, 0, new_position.z),	p1(center.x, 0, center.z);

				pushed_aside = p1 - p;
				t.m_position = pushed->position();
				t.m_position += pushed_aside;
				actions.push(pushed);

				// only moves the source a little bit
				pushed_aside = p - p1;
				tranform.m_position = new_position + pushed_aside;
				entity->transform(&tranform);
			}
			else {
				// both objects are non movable, deny move
				entity->popTransformations();				// restore previous position
				block_move = true;
			}
		}

		// only test ceiling collision if the entity is moving up
		if (nearest_ceiling != nullptr && new_position.y > old_position.y) {
			// if the entity hit the ceiling BUT can be pushed aside by another triangle
			gaEntity* collisionWith = static_cast<gaEntity*>(nearest_ceiling->m_source);

			if (collisionWith->movable()) {
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
				informCollision(collisionWith, entity, nearest_ceiling->m_position);
			}
			else {
				//non-movable entities always block the movement
				entity->popTransformations();				// restore previous position

				// stop ballistic move
				if (m_ballistics.count(entity->name()) > 0 && m_ballistics[entity->name()].m_inUse) {
					// if the object was going upward, force the position
					const fwAABBox &aabb = entity->modelAABB();
					
					tranform.m_position.y = nearest_ceiling->m_position.y - aabb.height();
					glm::vec3 v0(nearest_ceiling->m_position.x - old_position.x, 
						old_position.y - nearest_ceiling->m_position.y,
						nearest_ceiling->m_position.z - old_position.z);
					m_ballistics[entity->name()].reset(tranform.m_position, v0);

					actions.push(entity);	// fix the entity altitude
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

				g_gaWorld.sendMessage(
					static_cast<dfSuperSector*>(nearest_ceiling->m_source)->name(),
					entity->name(),
					gaMessage::Action::COLLIDE,
					0,
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
							// if more than epsilon, fix the position to the ground triangle, not the intersection point (rounding errors)
							tranform.m_position.y = nearest_ground->m_triangle[0].y;

							actions.push(entity);	// fix the entity altitude
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
					collidedEntity = static_cast<gaEntity*>(nearest_ground->m_source);
					entity->superSector(static_cast<dfSuperSector*>(collidedEntity));
				}
			}
			else {
				// if there is no floor
				bool falling = true;

				// check if there is a at STEP distance below the entity
				if (entity->canStep()) {
					float step = static_cast<gaActor*>(entity)->step();
	 				glm::vec3 down(new_position.x, new_position.y - step, new_position.z);
					float z = ifCollideWithSectorOrEntity(entity->position(), down, fwCollision::Test::WITHOUT_BORDERS, entity);
					if (z != INFINITY) {
						// we found a floor !
						if (tranform.m_position.y != z) {
							tranform.m_position.y = z;

							actions.push(entity);

							// fix the entity altitude and give it another try
							if (entity->name() == "player")
								gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " step down to ground " + std::to_string(tranform.m_position.x)
									+ " " + std::to_string(tranform.m_position.y)
									+ " " + std::to_string(tranform.m_position.z));

							continue;
						}
						else {
							falling = false;
						}
					}
				}

				if (falling) {
					// OK we definitely need to fall
					switch (tranform.m_flag) {
					case gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL:
						// if the entity wants to be informed of falling
						entity->popTransformations();			// restore previous position
						g_gaWorld.sendMessage(
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
							g_gaWorld.sendMessage(
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
					}
				}
			}
		}

		// accept the move if we do not fix the position
		if (!fix_y && !block_move) {
			g_gaWorld.sendMessage(
				entity->name(),
				entity->name(),
				gaMessage::Action::MOVE,
				0,
				nullptr
			);

			if (nearest_ground) {
				// register the sector the entity walks on
				dfSuperSector* ss;

				collidedEntity = static_cast<gaEntity*>(nearest_ground->m_source);
				ss = dynamic_cast<dfSuperSector*>(collidedEntity);
				if (ss != nullptr) {
					entity->superSector(ss);
				}
				else {
					dfSector* s = dynamic_cast<dfSector*>(collidedEntity);
					if (s != nullptr) {
						entity->superSector(s->supersector());
					}
				}
			}
			else {
				// if not walking on a supersector, run a full search
				std::vector<gaEntity*> ss;
				g_gaWorld.getEntities(GameEngine::ClassID::Sector, ss);
				for (auto sector : ss) {
					if (sector->isPointInside(new_position)) {
						entity->superSector(static_cast<dfSuperSector*>(sector));
					}
				}
			}
		}

		// inform all objects that are sitting on that one
		lifted = tranform.m_position.y - old_position.y;
		if (lifted < 0) {
			for (auto& entry : sittingOnTop) {
				// unless the object entered ballistic mode, then let it run individually
				if (m_ballistics.count(entry.first) > 0) {
					continue;
				}

				gaEntity* pushed = entry.second;
				if (pushed->movable()) {
					actions.push(pushed);
					GameEngine::Transform& t = pushed->transform();
					t.m_position = pushed->position();
					t.m_position.y += lifted;
				}
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
	for (auto& name : m_remove) {
		m_ballistics.erase(name);
	}
	m_remove.clear();

	for (auto& en : m_ballistics) {
		Ballistic& b = en.second;

		for (auto entity : g_gaWorld.m_entities[en.first]) {
			gaDebugLog(1, "GameEngine::Physics::update", entity->name());
			b.apply(delta, entity->pTransform());

			entity->sendMessage(entity->name(), 
				gaMessage::Action::WANT_TO_MOVE, 
				gaMessage::Flag::WANT_TO_MOVE_FALL, 
				entity->pTransform());
		}
	}
}

/**
 * add an object to the ballistic
 */
void GameEngine::Physics::addBallistic(gaEntity* entity, const glm::vec3& v0)
{
	const glm::vec3& pold = entity->position();
	glm::vec3 pnew = pold + v0;

	m_ballistics[entity->name()] = Ballistic(pnew, pold);
}

/**
 * record state of an object
 */
void GameEngine::Physics::recordState(const std::string& name, flightRecorder::Ballistic* record)
{
	m_ballistics[name].recordState(name, record);
}

/**
 * reload the state of an object
 */
void GameEngine::Physics::loadState(flightRecorder::Ballistic* object)
{
	m_ballistics[object->name] = Ballistic(object);
}
