#include "Physics.h"

#include <memory>

#include "gaWorld.h"
#include "gaMessage.h"
#include "gaEntity.h"
#include "gaCollisionPoint.h"
#include "gaDebug.h"

#include "../darkforces/dfSuperSector.h"

using namespace GameEngine;

Physics::Physics(gaWorld* world) :
	m_world(world)
{
}

/**
 * Test if the entity warped through a triangle
 */
bool Physics::warpThrough(gaEntity *entity, 
	const glm::vec3& old_position,
	Transform& tranform)
{
	// do a warpThrough quick test
	glm::vec3 warp;
	glm::vec3 center = entity->modelAABB().center();
	fwAABBox aabb_ws(entity->position() + center, old_position + center);
	std::vector<glm::vec3> warps;

	for (auto sector : m_world->m_sectors) {
		if (sector->collideAABB(aabb_ws)) {
			// do a warpTrough full test
			if (entity->warpThrough(sector->collider(), old_position, warp)) {
				warps.push_back(warp);
			}
		}
	}

	if (warps.size() > 0) {
		float nearest = 9999999;
		glm::vec3 near_c = glm::vec3(0);

		// find the nearest point
		for (auto& collision : warps) {
			if (glm::distance2(old_position, collision) < nearest) {
				nearest = glm::distance2(old_position, collision);
				near_c = collision;
			}
		}
		// and force the object there
		tranform.m_position = near_c;
		entity->transform(&tranform);
		m_world->sendMessage(
			entity->name(),
			entity->name(),
			gaMessage::Action::MOVE,
			0,
			nullptr
		);
		if (entity->name() == "player")
			gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " warp detected, fixed at " + std::to_string(tranform.m_position.x)
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
	// elevators don't need to be checked against sectors
	uint32_t size;
	if (entity->collideSectors()) {
		for (auto sector : m_world->m_sectors) {
			size = collisions.size();
			if (sector->collideAABB(entity->worldAABB()) &&
				entity->collide(sector->collider(), tranform.m_forward, tranform.m_downward, collisions))
			{
				for (auto i = size; i < collisions.size(); i++) {
					collisions[i].m_source = sector;
					collisions[i].m_class = gaCollisionPoint::Source::SECTOR;
				}
			}
		}
	}
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

	gaDebugLog(1, "gaWorld::wantToMove",
		entity->name() + " found collision " + msg
		+ "with " + collider
		+ " at " + std::to_string(collision.m_position.y)
	);
}

/**
 * An entity wants to move
 */
void Physics::moveEntity(gaEntity* entity, gaMessage* message)
{
	std::vector<gaCollisionPoint> collisions;

	std::queue<gaEntity*> actions;

	actions.push(entity);

	while (actions.size() != 0) {
		collisions.clear();

		entity = actions.front();
		actions.pop();
		GameEngine::Transform& tranform = entity->transform();
		glm::vec3 old_position = entity->position();

		entity->pushTransformations();
		entity->transform(&tranform);

		if (entity->name() == "player")
			gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " to " + std::to_string(tranform.m_position.x)
				+ " " + std::to_string(tranform.m_position.y)
				+ " " + std::to_string(tranform.m_position.z));

		if (entity->collideSectors()) {
			if (warpThrough(entity, old_position, tranform)) {
				continue;	// object warped through a triangle
			}
		}

		// collide against the entities
		testEntities(entity, tranform, collisions);

		// collide against the sectors
		testSectors(entity, tranform, collisions);

		// find the nearest collisions
		gaCollisionPoint* nearest_collision = nullptr;
		gaCollisionPoint* nearest_ground = nullptr;
		float distance = 99999999.0f;
		float ground = -9999999.0f;
		float d;
		float dBottom = 0;						// distance from worldAAB bottom to collision Y
		float dTop = 0;							// distance from worldAAB top to collision Y
		float lifted = 0;						// if collision on top for elevator, record the Y lifting
		glm::vec3 pushed_aside = glm::vec3(0);	// if collision on border of elevator, record the XZ direction
		bool fall = false;						// are we falling
		bool fix_y = false;						// do we enter the ground and need Y to be fixed

		std::map<std::string, gaEntity*>& sittingOnTop = entity->sittingOnTop();

		for (auto& collision : collisions) {
			switch (collision.m_location) {
			case fwCollisionLocation::FRONT:
				d = entity->distanceTo(collision.m_position);
				if (d < distance) {
					nearest_collision = &collision;
					distance = d;
				}
				fall = false;
				if (entity->name() == "player") {
					gaDebugLog(1, "gaWorld::wantToMove", "FRONT detected at " + std::to_string(collision.m_position.y));
				}
				break;
			case fwCollisionLocation::BOTTOM:
				fall = false;
				if (message->m_value != gaMessage::Flag::WANT_TO_MOVE_LASER) {
					d = abs(tranform.m_position.y - collision.m_position.y);
					if (d > ground) {
						nearest_ground = &collision;
						ground = d;
					}
				}
				if (entity->name() == "player") {
					gaDebugLog(1, "gaWorld::wantToMove", "BOTTOM detected at " + std::to_string(collision.m_position.y));
				}
				break;
			case fwCollisionLocation::COLLIDE: {
				// cylinder collision (player)
				// position of the intersection compared to the direction
				const fwAABBox& worldAABB = entity->worldAABB();

				dBottom = collision.m_position.y - worldAABB.m_p.y;
				dTop = worldAABB.m_p1.y - collision.m_position.y;

				if (dBottom < 0.101) {				// BOTTOM
					if (entity->name() == "player")
						debugCollision(collision, entity, "on bottom");

					if (dBottom > ground) {
						nearest_ground = &collision;
						ground = dBottom;
					}
				}
				else if (dTop < 0.101) {			// TOP
					if (entity->name() == "player")
						debugCollision(collision, entity, "on top");

					d = entity->distanceTo(collision.m_position);
					if (d < distance) {
						nearest_collision = &collision;
						distance = d;
					}

					lifted = tranform.m_position.y - old_position.y;

					// add the other entity on the list of action
					if (collision.m_class == gaCollisionPoint::Source::ENTITY &&
						message->m_value == gaMessage::Flag::PUSH_ENTITIES
						) {
						gaEntity* top = static_cast<gaEntity*>(collision.m_source);
						actions.push(top);

						GameEngine::Transform& t = top->transform();
						t.m_position = top->position();
						t.m_position.y += lifted;

						if (sittingOnTop.count(top->name()) == 0) {
							sittingOnTop[top->name()] = top;
						}
					}
				}
				else {							// FRONT/BACK/LEFT/RIGHT
					if (entity->name() == "player")
						debugCollision(collision, entity, "on edge");

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

		// manage ground collision and accept to jump up if over a step
		if (entity->gravity()) {
			if (nearest_ground) {
				if (ground < 0.101) {
					if (ground != 0) {
						tranform.m_position.y = nearest_ground->m_position.y;
						actions.push(entity);	// fix the entity altitude
						fix_y = true;
					}

					// inform the other entity we are sitting on top
					if (nearest_ground->m_class == gaCollisionPoint::Source::ENTITY) {
						gaEntity* top = static_cast<gaEntity*>(nearest_ground->m_source);
						std::map<std::string, gaEntity*>& _sittingOnTop = top->sittingOnTop();

						if (_sittingOnTop.count(entity->name()) == 0) {
							_sittingOnTop[entity->name()] = entity;
						}
					}

					if (entity->name() == "player")
						gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " found ground at " + std::to_string(nearest_ground->m_position.y));
				}
				else {
					// convert the down collision to a nearest
					nearest_collision = nearest_ground;
				}
			}
			else {
				// if there is no floor
				switch (message->m_value) {
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
					// if the entity wants to be informed of falling
					m_world->sendMessage(
						entity->name(),
						entity->name(),
						gaMessage::Action::FALL,
						0,
						nullptr
					);
					if (entity->name() == "player")
						gaDebugLog(1, "gaWorld::wantToMove", entity->name() + "falling");
					continue;
				}
			}
		}

		if (nearest_collision) {
			// if there is a collision

			if (message->m_value == gaMessage::Flag::PUSH_ENTITIES) {
				// inform being lifted or pushed aside
				if (pushed_aside != glm::vec3(0)) {
					if (nearest_collision->m_class == gaCollisionPoint::Source::ENTITY) {
						// push another entity aside
						gaEntity* pushed = static_cast<gaEntity*>(nearest_collision->m_source);
						actions.push(pushed);

						if (entity->name() == "player")
							gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " pushes " + pushed->name());

						GameEngine::Transform& t = pushed->transform();
						t.m_position = entity->position();
						t.m_position += pushed_aside;
					}
					else {
						// being pushed by a sector
						tranform.m_position.x += pushed_aside.x;
						tranform.m_position.z += pushed_aside.z;
						actions.push(entity);

						if (entity->name() == "player")
							gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " pushed by sector");
					}
				}
			}

			if (message->m_value != gaMessage::Flag::PUSH_ENTITIES) {
				if (entity->name() == "player")
					gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " deny move " + std::to_string(tranform.m_position.x)
					+ " " + std::to_string(tranform.m_position.y)
					+ " " + std::to_string(tranform.m_position.z));

				// and we do not force the move
				// refuse the move and inform both element from the collision
				entity->popTransformations();				// restore previous position

				if (nearest_collision->m_class == gaCollisionPoint::Source::ENTITY) {
					m_world->sendMessage(
						static_cast<gaEntity*>(nearest_collision->m_source)->name(),
						entity->name(),
						gaMessage::Action::COLLIDE,
						gaMessage::Flag::COLLIDE_ENTITY,
						nullptr
					);
				}
				else {
					m_world->sendMessage(
						static_cast<dfSuperSector*>(nearest_collision->m_source)->name(),
						entity->name(),
						gaMessage::Action::COLLIDE,
						gaMessage::Flag::COLLIDE_WALL,
						nullptr
					);
				}
				continue;
			}
		}

		// accept the move
		if (!fix_y) {
			m_world->sendMessage(
				entity->name(),
				entity->name(),
				gaMessage::Action::MOVE,
				0,
				nullptr
			);
		}
		else {
			if (entity->name() == "player")
				gaDebugLog(1, "gaWorld::wantToMove", entity->name() + " fixed ground " + std::to_string(tranform.m_position.x)
				+ " " + std::to_string(tranform.m_position.y)
				+ " " + std::to_string(tranform.m_position.z));

		}

		// inform all objects that are sitting on that one
		lifted = tranform.m_position.y - old_position.y;
		if (lifted < 0) {
			for (auto entry : sittingOnTop) {
				gaEntity* pushed = entry.second;
				actions.push(pushed);

				GameEngine::Transform& t = pushed->transform();
				t.m_position = entity->position();
				t.m_position.y += lifted;
			}
			sittingOnTop.clear();	// clear the cache, it will be reimplemented by the object if it is still on top
		}
	}
}

