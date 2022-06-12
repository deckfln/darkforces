#include "Physics.h"

#include <memory>
#include <map>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/intersect.hpp>

#include "../framework/geometries/fwGeometrySphere.h"
#include "World.h"
#include "gaMessage.h"
#include "gaEntity.h"
#include "gaCollisionPoint.h"
#include "gaDebug.h"
#include "gaActor.h"
#include "gaComponent/gaCImposter.h"

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
bool Physics::warpThrough(gaEntity* entity,
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
		gaEntity* collided = nullptr;

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
			entity->undoTransform();

			// or move away from the bad position
//			glm::vec3 d = glm::normalize(near_c - old_position);
//			d.y = 0;
//			tranform.m_position += d * entity->radius();
//			
		}

		// always inform the source entity
		//informCollision(collided, entity, near_c);

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
	gaEntity* target = nullptr;

	for (auto& entry : g_gaWorld.m_entitiesByID) {
		target = entry.second;

		// ignore ghosts
		if (!target->hasCollider()) {
			continue;
		}

		if (!entity->collideAABB(target->worldAABB())) {
			continue;
		}

		if (entity->name() == "player" && target->name() == "enthall;blocker1;72;70;blocker2;69;67;watchwhat;") {
			printf("Physics::testEntities blocker2 vs player\n");
		}

		if (target->name() != entity->name()) {
			size = collisions.size();
			if (entity->collide(target, tranform.m_forward, tranform.m_downward, collisions)) {
				
				for (auto i = size; i < collisions.size(); i++) {
					collisions[i].m_source = target;
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
void Physics::informCollision(
	gaMessage* msg, 
	gaEntity* from, 
	gaEntity* to, 
	const glm::vec3& collision, 
	const std::vector<gaEntity*>& verticalCollision)
{
	// always inform the source entity 
	gaMessage* message  = g_gaWorld.sendMessage(
		from->name(),
		to->name(),
		gaMessage::Action::COLLIDE,
		collision,
		nullptr
	);

	// record the original want_to_move id
	message->m_value = msg->m_id;

	if (verticalCollision.size() > 0) {
		// complete the onboard data
		struct CollisionList* data = (struct CollisionList*)&message->m_data[0];
		data->size = verticalCollision.size();

		if (sizeof(struct Physics::CollisionList) + sizeof(gaEntity*) * data->size >= sizeof(message->m_data)) {
			data->size = (sizeof(message->m_data) - sizeof(uint32_t)) / sizeof(gaEntity*);
			__debugbreak();
		}

		for (uint32_t i = 0; i < data->size; i++) {
			data->entities[i] = verticalCollision[i];

			// always inform the colliding entity 
			message = g_gaWorld.sendMessage(
				to->name(),
				verticalCollision[i]->name(),
				gaMessage::Action::COLLIDE,
				collision,
				nullptr
			);

			// record the original want_to_move id
			message->m_value = msg->m_id;
		}
	}
	else {
		gaMessage* message = g_gaWorld.sendMessage(
			to->name(),
			from->name(),
			gaMessage::Action::COLLIDE,
			collision,
			nullptr
		);
	}
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

	/*
	gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " to " + std::to_string(tranform.m_position.x)
		+ " " + std::to_string(tranform.m_position.y)
		+ " " + std::to_string(tranform.m_position.z));
	*/

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
				// send a near hit
				ent->sendMessage(gaMessage::Action::BULLET_MISS);
				continue;
			}

			// extended test
			collisions.clear();
			if (ent->collide(collider, forward, down, collisions)) {
				for (auto& c : collisions) {
					len = glm::distance2(old_position, c.m_position);
					if (len < min_len) {
						// record a hit, but keep only the nearest to the bullet
						collision = c.m_position;
						min_len = len;
						collidedEntity = ent;
					}
					else if (collidedEntity == nullptr || collidedEntity != ent) {
						// send a near hit
						ent->sendMessage(gaMessage::Action::BULLET_MISS);
					}
				}
			}
		}
	}

	if (collidedEntity != nullptr) {
		// refuse the move and inform ONLY the bullet of the collision
		entity->undoTransform();				// restore previous position

		collidedEntity->sendMessage(
			entity->name(),
			gaMessage::Action::COLLIDE,
			collision,
			nullptr
		);
		return;
	}

	// acknowledge the move
	g_gaWorld.sendMessage(
		entity->name(),
		entity->name(),
		gaMessage::Action::MOVE,
		(int)message->m_id,				// WANT_TO_MOVE that triggered the event
		nullptr
	);
}

struct Solver {
	glm::vec3 collision;
};

struct phMessage {
	uint32_t m_message;
	uint32_t m_ivalue;
	void *m_extra;
};
/**
 * An entity wants to move
 */
void Physics::moveEntity(gaEntity* entity, gaMessage* message)
{
	std::map<gaEntity*, std::map<gaEntity*, glm::vec3>> solvers;	// list of entities moving (or trying to move)
	std::map<gaEntity*, glm::mat4x4> originalMatrices;				// source position of each object

	m_entityMsg.clear();

	// bullets are special case
	if (message->m_value == gaMessage::Flag::WANT_TO_MOVE_LASER) {
		moveBullet(entity, message);
		return;
	}

	std::vector<gaCollisionPoint> collisions;

	std::queue<gaEntity *> actions;
	std::map<std::string, bool> pushedEntities;	// list of entities we are pushing, only push an entity once per run
	std::vector<gaEntity*> verticalCollision;
	GameEngine::Component::Imposter* pMover=nullptr;
	GameEngine::Component::Imposter* pCollided = nullptr;

	// kick start actions
	actions.push(entity);

	bool first = true;
	while (actions.size() != 0) {
		collisions.clear();

		entity = actions.front();

		actions.pop();

		GameEngine::Transform& tranform = entity->transform();

		// as the objects is moving remove references to previous 'sitting on'
		pMover = dynamic_cast<GameEngine::Component::Imposter*>(entity->findComponent(gaComponent::Imposter));
		if (pMover) {
			pMover->clearEntitiesBelow();
		}

		// register the original position of the entity
		if (originalMatrices.count(entity) == 0) {
			originalMatrices[entity] = entity->worldMatrix();
			entity->pushTransformations();
		}

		// extract the position of the entity during THAT run
 		glm::vec3 old_position = entity->position();

#ifdef _DEBUG
		if (entity->name() == "OFFCFIN.WAX(21)") {
			gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " to " + std::to_string(tranform.m_position.x)
				+ " " + std::to_string(tranform.m_position.y)
				+ " " + std::to_string(tranform.m_position.z));
		}
#endif

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

		// non-collider object do not trigger collisions, unles they are falling
		if (!entity->hasCollider()) {
			g_gaWorld.sendMessage(
				entity->name(),
				entity->name(),
				gaMessage::Action::MOVE,
				(int)message->m_id,				// WANT_TO_MOVE that triggered the event
				nullptr
			);
			continue;
		}

//		if (entity->falling()) {
		// collide against the entities
		testEntities(entity, tranform, collisions);
//		}

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

		verticalCollision.clear();

		gaEntity* collidedEntity;

		for (auto& collision : collisions) {
			collidedEntity = static_cast<gaEntity*>(collision.m_source);
		
			switch (collision.m_location) {
			case fwCollisionLocation::FRONT: {
				d = entity->distanceTo(collision.m_position);
				if (d < distance) {
					nearest_collision = &collision;
					distance = d;
				}
				fall = false;

				bool b = false;
				for (auto k : verticalCollision) {
					if (k == collidedEntity) {
						b = true;
						break;
					}
				}
				if (!b) {
					verticalCollision.push_back(collidedEntity);
				}
				break;	}

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
						solvers[entity][collidedEntity] = collision.m_position;
						solvers[collidedEntity][entity] = collision.m_position;

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

						// optimization for the physics, link together entities sitting on top of each others
						pCollided = dynamic_cast<GameEngine::Component::Imposter*>(collidedEntity->findComponent(gaComponent::Imposter));

						fwAABBox& entityAABB = (fwAABBox&)entity->worldAABB();
						const fwAABBox& colliderAABB = collidedEntity->worldAABB();
						if (entityAABB.isAbove(colliderAABB)) {
							if (pCollided) {
								pCollided->addEntityOnTop(entity);
							}
							if (pMover) {
								pMover->addEntityBelow(collidedEntity);
							}
						}
						else {
							if (pMover) {
								pMover->addEntityOnTop(collidedEntity);
							}
							if (pCollided) {
								pCollided->addEntityBelow(entity);
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

					bool b = false;
					for (auto k : verticalCollision) {
						if (k == collidedEntity) {
							b = true;
							break;
						}
					}
					if (!b) {
						verticalCollision.push_back(collidedEntity);
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
		}	// end of check all collisions

		// manage ground collision and accept to jump up if over a step
		if (entity->gravity()) {

			if (nearest_ground) {
				float deltaY = new_position.y - nearest_ground->m_position.y;	// distance to the ground
				if (m_ballistics.count(entity->name()) > 0 && m_ballistics[entity->name()].m_inUse) {
					// if the object was falling, remove from the list and force the position
					m_remove.push_back(entity->name());
					entity->falling(false);
					m_ballistics[entity->name()].m_inUse = false;

					tranform.m_position.y = nearest_ground->m_triangle[0].y;
					actions.push(entity);

					if (entity->name() == "player")
						gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " falling to ground " + std::to_string(tranform.m_position.x)
							+ " " + std::to_string(tranform.m_position.y)
							+ " " + std::to_string(tranform.m_position.z));

					continue;	// deal directly with the request
				}
				else if (abs(deltaY) < EPSILON) {
					// NOP as we are 'just' on the surface
				}
				else if (deltaY < static_cast<gaActor*>(entity)->step()) {
					if (entity->canStep()) {
						// if there is a step and the entity can step over
						if (abs(nearest_ground->m_position.y - new_position.y) > EPSILON) {
							// if more than epsilon, fix the position to the ground triangle, not the intersection point (rounding errors)
							tranform.m_position.y = nearest_ground->m_triangle[0].y;

							actions.push(entity);	// fix the entity altitude

							if (entity->name() == "player")
								gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " fixed ground " + std::to_string(tranform.m_position.x)
									+ " " + std::to_string(tranform.m_position.y)
									+ " " + std::to_string(tranform.m_position.z));
							continue;	// deal directly with the request
						}
					}
					else {
						// convert the down collision to a nearest
						gaEntity* down = static_cast<gaEntity*>(nearest_ground->m_source);
						if (down->physical()) {
							if (entity->name() == "player")
								gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " deny move " + std::to_string(tranform.m_position.x)
									+ " " + std::to_string(tranform.m_position.y)
									+ " " + std::to_string(tranform.m_position.z));

							entity->undoTransform();				// restore previous position
							block_move = true;
						}
						else {
							if (entity->name() == "player")
								gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " collide " + std::to_string(tranform.m_position.x)
									+ " " + std::to_string(tranform.m_position.y)
									+ " " + std::to_string(tranform.m_position.z));
						}

						solvers[entity][down] = nearest_ground->m_position;
						solvers[down][entity] = nearest_ground->m_position;
					}
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

							// give it an other try
							continue; // deal directly with the request
						}
						else {
							falling = false;
						}
					}
				}

				if (falling) {
					// OK we definitely need to fall
					switch (tranform.m_flag) {
					case gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL: {

						if (entity->name() == "player")
							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " would fall " + std::to_string(tranform.m_position.x)
								+ " " + std::to_string(tranform.m_position.y)
								+ " " + std::to_string(tranform.m_position.z));

						// if the entity wants to be informed of falling
						entity->undoTransform();			// restore previous position
						block_move = true;

						entity->sendMessage(entity->name(), gaMessage::WOULD_FALL, message->m_id);
						break; }

					default:
						if (m_ballistics.count(entity->name()) == 0) {
							// if the entity wants to be informed of falling
							entity->sendMessage(entity->name(), gaMessage::FALL, message->m_id);

							// engage ballistic
							entity->falling(true);
							m_ballistics[entity->name()] = Ballistic(tranform.m_position, old_position);
						}

						if (entity->name() == "player")
							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " falling");
					}
					// check lateral collision even when falling
				}
			}
		}

		// manage lateral collision
		if (nearest_collision) {
			// if there is a collision
			gaEntity* pushed = static_cast<gaEntity*>(nearest_collision->m_source);

			if (entity->name() == "player")
				gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " trying to pushe");

			if (!entity->movable() && pushed->movable()) {

				if (entity->name() == "player")
					gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " pushes " + pushed->name());

				GameEngine::Transform& t = pushed->transform();
				const glm::vec3& p = pushed->position();
				const glm::vec3 p1(nearest_collision->m_position.x, pushed->position().y, nearest_collision->m_position.z);

				pushed_aside = glm::normalize(p - p1) * (pushed->radius() + EPSILON);
				t.m_position = p1 + pushed_aside;
				t.m_scale = pushed->get_scale();
				t.m_quaternion = pushed->quaternion();

				// check if the collision is already registered, means the entity is moving between 2 imuatble enntity
				float d = glm::length2(nearest_collision->m_position - solvers[entity][pushed]);
				if (d < EPSILON) {
					entity->undoTransform();				// restore previous position
					block_move = true;
					__debugbreak();
				}
				else {
					solvers[pushed][entity] = nearest_collision->m_position;
					solvers[entity][pushed] = nearest_collision->m_position;

					// the entity (non movable) pushes the collided (movable)
					actions.push(pushed);
				}
			}
			else if (entity->movable() && !pushed->movable()) {

				if (entity->name() == "player")
					gaDebugLog(1, "GameEngine::Physics::wantToMove", pushed->name() + " pushes " + entity->name());

				GameEngine::Transform& t = entity->transform();
				const glm::vec3& p = entity->position();
				const glm::vec3 p1(nearest_collision->m_position.x, entity->position().y, nearest_collision->m_position.z);

				pushed_aside = glm::normalize(p - p1) * (entity->radius() + EPSILON);
				t.m_position = p1 + pushed_aside;
				t.m_scale = entity->get_scale();
				t.m_quaternion = entity->quaternion();

				// check if the collision is already registered, means the entity is moving between 2 imutable enntities
				float d = glm::length2(nearest_collision->m_position - solvers[entity][pushed]);
				if (d < EPSILON) {
					entity->undoTransform();				// restore previous position
					block_move = true;
				}
				else {
					solvers[entity][pushed] = nearest_collision->m_position;
					solvers[pushed][entity] = nearest_collision->m_position;

					// the entity (non movable) pushes the collided (movable)
					actions.push(entity);
				}

				/*
				// the entity (movable) is being pushed by collided (non movable)
				// push by an average radius
				float radius = entity->radius();

				glm::vec2 p_collision(nearest_collision->m_position.x, nearest_collision->m_position.z);
				glm::vec2 p_old(new_position.x, new_position.z);

				float l1 = glm::distance(p_collision, p_old);
				glm::vec2 delta2d = glm::normalize(p_collision - p_old);
				glm::vec2 direction2d(direction.x, direction.z);

				float dot = glm::dot(direction2d, delta2d);
				delta2d *= (radius + EPSILON);
				float d = glm::length(delta2d);

				// if the moved back position is nearly the same as the original position, block the movement
				if (dot < 0 || glm::length(delta2d) < EPSILON) {
					// ONLY refuse the move if the entity is a physical one
					if (pushed->physical()) {
						if (entity->name() == "player")
  							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " deny move " + std::to_string(tranform.m_position.x)
								+ " " + std::to_string(tranform.m_position.y)
								+ " " + std::to_string(tranform.m_position.z));

						entity->undoTransform();				// restore previous position
						block_move = true;

						if (m_ballistics.count(entity->name()) != 0) {
							// if the object is jumping or falling change the balistic
							m_ballistics[entity->name()] = Ballistic(old_position, old_position);
						}
					}
					else {
						if (entity->name() == "player")
							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " collide " + std::to_string(tranform.m_position.x)
								+ " " + std::to_string(tranform.m_position.y)
								+ " " + std::to_string(tranform.m_position.z));
					}

					// always inform the source entity
					informCollision(message, pushed, entity, nearest_collision->m_position, verticalCollision);
				}
				else {
					glm::vec3 p1(nearest_collision->m_position.x, new_position.y, nearest_collision->m_position.z);
					tranform.m_position = p1 - glm::vec3(
						nearest_collision->m_position.x - new_position.x, 
						0, 
						nearest_collision->m_position.z - new_position.z);

					const std::string& name = entity->name();

					if (m_ballistics.count(name) > 0 && m_ballistics[name].m_inUse) {
						glm::vec3 v0(0, new_position.y - old_position.y, 0);

						m_ballistics[name].reset(tranform.m_position, v0);
					}

					if (glm::length(tranform.m_position - old_position) > EPSILON) {
						entity->transform(&tranform);

						if (name == "player") {
							gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " pushed by sector");
							//printf("%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\n",	new_position.x, new_position.z,nearest_collision->m_position.x, nearest_collision->m_position.z,	delta2d.x, delta2d.y);
						}
					}
				}
				*/
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
				actions.push(entity);

				solvers[entity][pushed] = nearest_collision->m_position;
				solvers[pushed][entity] = nearest_collision->m_position;

				if (entity->name() == "player")
					gaDebugLog(1, "GameEngine::Physics::wantToMove", pushed->name() + " collision " + entity->name());
			}
			else {
 				// both objects are non movable, deny move
				entity->undoTransform();				// restore previous position
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

					entity->undoTransform();				// restore previous position
				}
				else {
					if (entity->name() == "player")
						gaDebugLog(1, "GameEngine::Physics::wantToMove", entity->name() + " collide upward " + std::to_string(tranform.m_position.x)
							+ " " + std::to_string(tranform.m_position.y)
							+ " " + std::to_string(tranform.m_position.z));
				}

				// always inform the source entity 
				informCollision(message, collisionWith, entity, nearest_ceiling->m_position, verticalCollision);
			}
			else {
				//non-movable entities always block the movement
				entity->undoTransform();				// restore previous position

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

		// accept the move if we do not fix the position
		if (!fix_y && !block_move) {
			// also send a rotation if requested
			if (tranform.m_rotate) {
				g_gaWorld.sendMessage(
					entity->name(),
					entity->name(),
					gaMessage::Action::ROTATE,
					gaMessage::Flag::ROTATE_QUAT,					// WANT_TO_MOVE that triggered the event
					&tranform.m_quaternion
				);
			}

			// register the sector the entity walks on if the entity is sensible to gravity
			if (entity->gravity()) {
				if (nearest_ground) {
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
		}

		// inform all objects that are sitting on that one
		lifted = tranform.m_position.y - old_position.y;
		if (lifted < 0) {
			if (pMover) {
				const std::vector<gaEntity*>& entities = pMover->entitiesOnTop();
				for (auto entity : entities) {
					if (entity->movable()) {
						actions.push(entity);
						GameEngine::Transform& t = entity->transform();
						t.m_position = entity->position();
					}
				}
				pMover->clearEntitiesOnTop();
			}
		}
	}

	// inform entities that collided
	for (auto& entry : solvers) {
		gaEntity* entity = entry.first;

		gaMessage msg(entity->name(), entity->name());
		struct CollisionList* data = (struct CollisionList*)&msg.m_data[0];

		msg.m_action = gaMessage::Action::COLLIDE;
		data->size = entry.second.size();

#ifdef _DEBUG
		// ensure there is no out of buffer
		if (sizeof(struct Physics::CollisionList) + sizeof(gaEntity*) * data->size >= sizeof(msg.m_data)) {
			__debugbreak();
		}
#endif

		uint32_t i = 0;
		for (auto& collision : entry.second) {
			gaEntity* collided = collision.first;

			// check duplicated
			data->entities[i] = collided;
			data->collision[i] = collision.second;
			i++;
		}
		g_gaWorld.sendMessage(&msg);
	}

	// inform entities that moved
	for (auto& entry : originalMatrices) {
		gaEntity* entity = entry.first;
		const glm::mat4x4& matrix = entity->worldMatrix();
		if (entry.second == matrix) {
			// the entity was handled by cannot moved
			gaMessage *msg = entry.first->sendMessage(entity->name(), gaMessage::Action::CANT_MOVE);
			struct CollisionList* data = (struct CollisionList*)&msg->m_data[0];

			uint32_t i = 0;
			data->size = solvers[entity].size();
			for (auto& collision : solvers[entity]) {
				gaEntity* collided = collision.first;

				// check duplicated
				data->entities[i] = collided;
				data->collision[i] = collision.second;
				i++;

				msg->m_v3value = collision.second;
			}
		}
		else {
			// the entity moved
			entry.first->sendMessage(entity->name(), gaMessage::Action::MOVE, 0, nullptr);
		}
	}
}

/**
 * Deal with falling objects
 */
void Physics::update(time_t delta)
{
	for (auto& name : m_remove) {
		for (auto entity : g_gaWorld.m_entities[name]) {
			entity->falling(false);
		}

		m_ballistics.erase(name);
	}
	m_remove.clear();

	for (auto& en : m_ballistics) {
		Ballistic& b = en.second;

		for (auto entity : g_gaWorld.m_entities[en.first]) {
			gaDebugLog(1, "GameEngine::Physics::update", entity->name());
			b.apply(delta, entity->pTransform());

			g_gaWorld.sendMessage("_physics", 
				entity->name(), 
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

	entity->falling(true);
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
