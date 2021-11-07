#include "dfCSpriteAnimated.h"

#include <glm/trigonometric.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

#include <imgui.h>

#include "../../config.h"

#include "../../framework/geometries/fwGeometryCylinder.h"

#include "../../gaEngine/World.h"
#include "../../gaEngine/gaComponent/gaCActor.h"

#include "../../dfModel/dfWAX.h"
#include "../../dfLevel.h"
#include "../../dfComponent.h"
#include "../../dfComponent/dfComponentLogic.h"

static glm::vec4 g_red(1.0, 1.0, 0.0, 1.0);
static fwMaterialBasic g_basic(&g_red);
static fwGeometryCylinder* g_view = nullptr;

static std::map<dfState, bool> g_loopStates{
	{ dfState::ENEMY_ATTACK, false}
};

#ifdef _DEBUG
/**
 * add/remove a vector mesh
 */
void DarkForces::Component::SpriteAnimated::directionVector(void)
{
	if (m_debug) {
		// add a point of view vector
		if (m_view == nullptr) {
			m_view = new fwMesh(g_view, &g_basic);
		}

		float a = atan2(m_direction.x, m_direction.z);// +M_PI / 2.0f;
		glm::vec3 xr(0, a, 0);
		m_view->rotate(xr);
		m_view->translate(m_entity->position() + glm::vec3(0.0f, 0.5f, 0.0f));

		g_gaWorld.add2scene(m_view);
	}
	else {
		g_gaWorld.remove2scene(m_view);
	}
}
#endif

DarkForces::Component::SpriteAnimated::SpriteAnimated():
	DarkForces::Component::Sprite()
{
}

/**
 * create a sprite from a pointer to a model
 */
DarkForces::Component::SpriteAnimated::SpriteAnimated(dfWAX* wax, float ambient) :
	DarkForces::Component::Sprite(wax, ambient)
{
	if (g_view == nullptr) {
		g_basic.makeStatic();
		g_view = new fwGeometryCylinder(0.01f, 0.15f, 4, 1);
		g_view->makeStatic();
	}
}

/**
 * create a sprite from a model name, the real model is extracted from the world
 */
DarkForces::Component::SpriteAnimated::SpriteAnimated(const std::string& model, float ambient) :
	DarkForces::Component::Sprite((dfWAX*)g_gaWorld.getModel(model), ambient)
{
}

/**
 * Change the state of an object
 */
void DarkForces::Component::SpriteAnimated::state(dfState state)
{
	// already on that state in a running loop
	if (state == dfState::NONE || (state == m_state && m_animated == true)) {
		return;
	}

	m_state = state;
	m_entity->modelAABB(((dfWAX*)m_source)->bounding(m_state));
	m_entity->updateWorldAABB();
	m_lastFrame = m_currentFrame = 0;
	m_frame = 0;
	m_dirtyAnimation = true;

	if (g_loopStates.find(state) != g_loopStates.end()) {
		m_loopAnimation = g_loopStates[state];
	}
	else {
		m_loopAnimation = true;
	}

	m_nbframes = m_source->nbFrames(m_state);
	uint32_t frameRate = m_source->framerate(m_state);

	dfComponentLogic* logic = dynamic_cast<dfComponentLogic*>(m_entity->findComponent(DF_COMPONENT_LOGIC));

	if (logic->logic() & DF_LOGIC_ENEMIES) {
		// trigger the animation, unless the object is static or has no animation
		if (m_state != dfState::ENEMY_LIE_DEAD &&
			m_state != dfState::ENEMY_STAY_STILL)
		{
			m_entity->sendMessage(DarkForces::Message::ANIM_START, (uint32_t)m_state, &m_nbframes);
			m_animated = true;
			m_entity->timer(true);	// register to timer events
		}
	}
	else {
		// trigger an animation loop if there is actually an animation loop
		if ((logic->logic() & dfLogic::ANIM)) {
			m_entity->sendMessage(DarkForces::Message::ANIM_START, (uint32_t)m_state, &m_nbframes);
			m_animated = true;
			m_entity->timer(true);
		}
		else {
			m_animated = false;
			m_entity->timer(false);	// de-register from timer events
		}
	}
}

/**
 * save and restore status
 */
void DarkForces::Component::SpriteAnimated::pushState(dfState _state)
{
	m_previousStates.push(m_state);
	state(_state);
}

dfState DarkForces::Component::SpriteAnimated::popState(void)
{
	dfState _state = m_previousStates.top();
	m_previousStates.pop();
	state(_state);
	return _state;
}

/**
 *
 */
bool DarkForces::Component::SpriteAnimated::update(glm::vec3* position, glm::vec4* texture, glm::vec3* direction)
{
	if (m_dirtyPosition) {
		*direction = m_direction;
	}

	if (m_dirtyAnimation) {
		texture->g = (float)m_state;
		texture->b = (float)m_frame;
	}

	return DarkForces::Component::Sprite::update(position, texture, direction);
}

/**
 * Create a direction vector ased on pch, yaw, rol
 */
void DarkForces::Component::SpriteAnimated::rotation(const glm::vec3& rotation)
{
	// YAW = value in degrees where 0 is at the "top of the screen when you look at the map". The value increases clockwise
	float yaw = glm::radians(rotation.y);
	m_direction.x = -sin(yaw);	// in level space
	m_direction.y = 0;
	m_direction.z = cos(yaw);

#ifdef _DEBUG
	if (m_view) {
		float a = atan2(m_direction.x, m_direction.z);// +M_PI / 2.0f;
		glm::vec3 xr(0, a, 0);
		m_view->rotate(xr);
	}
#endif

	GameEngine::Component::Actor* actor = dynamic_cast<GameEngine::Component::Actor*>(m_entity->findComponent(gaComponent::Actor));
	if (actor) {
		actor->direction(m_direction);
	}
	m_dirtyPosition = true;
}

/**
 * Animate the object frame
 */
bool DarkForces::Component::SpriteAnimated::update(time_t t)
{
	dfComponentLogic* logic = dynamic_cast<dfComponentLogic*>(m_entity->findComponent(DF_COMPONENT_LOGIC));
	if (!(logic->logic() & dfLogic::ANIM)) {
		return false;
	}

	m_currentFrame += t;

	/*
	if (m_nbframes <= 1) {
		return false;	// static objects like FME are not updated,
						// objects with only 1 frame in the animation loop are also not updated
	}
	*/

	uint32_t frameRate = m_source->framerate(m_state);
	time_t frameTime = 1000 / frameRate; // time of one frame in milliseconds

	time_t delta = m_currentFrame - m_lastFrame;
	if (delta >= frameTime) {
		m_frame = m_source->nextFrame(m_state, m_frame);

		if (m_frame == -1) {
			return false;	// reached end of animation loop
		}

		m_entity->sendMessage(DarkForces::Message::ANIM_NEXT_FRAME, m_frame);

		m_lastFrame = (m_currentFrame / frameTime) * frameTime;
		m_dirtyAnimation = true;
	}

	return true;	// continue animation loop
}

/**
 * Deal with animation messages
 */
void DarkForces::Component::SpriteAnimated::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::WORLD_INSERT:
		state(m_state);
		break;

	case DarkForces::Message::STATE:
		state((dfState)message->m_value);
		break;

	case gaMessage::TIMER:
		if (!m_animated) {
			// ignore timer ticks when there is no ongoing animation
			return;
		}

		if (!update(message->m_delta)) {

			// go for next animation if the animation loop ?
			if (m_loopAnimation && m_source->nbFrames(m_state) > 1) {
				// restart the counters
				m_lastFrame = m_currentFrame = 0;
				m_frame = 0;
				m_dirtyAnimation = true;
				m_entity->sendMessage(DarkForces::Message::END_LOOP, (uint32_t)m_state);
			}
			else {
				m_entity->sendMessage(DarkForces::Message::ANIM_END, (uint32_t)m_state);
				m_animated = false;
				m_entity->timer(false);
			}
		}
		break;

	case DarkForces::Message::ROTATE:
		rotation(message->m_v3value);
		break;

	case gaMessage::LOOK_AT:
		if (message->m_extra == nullptr) {
			m_direction = glm::normalize(message->m_v3value);
			m_dirtyPosition = true;
#ifdef _DEBUG
			if (m_view) {
				float a = atan2(m_direction.x, m_direction.z);// +M_PI / 2.0f;
				glm::vec3 xr(0, a, 0);

				m_view->rotate(xr);
			}
#endif
		}
		break;

	case DarkForces::Message::START_FIRE:
		state(dfState::ENEMY_ATTACK);
		break;

	case gaMessage::START_MOVE:
		state(dfState::ENEMY_MOVE);
		break;

	case gaMessage::END_MOVE:
		if (m_state == dfState::ENEMY_MOVE) {
			state(dfState::ENEMY_STAY_STILL);
		}
		break;

	case gaMessage::MOVE:
		if (m_view) {
			m_view->translate(m_entity->position() + glm::vec3(0.0f, 0.5f, 0.0f));
		}
		break;
	}
	DarkForces::Component::Sprite::dispatchMessage(message);
}

/**
 * Add dedicated component debug the entity
 */
static std::map < uint32_t, const char*> debugStatesEnemies = {
	{0, "ENEMY_MOVE"},
	{1, "ENEMY_ATTACK" },
	{2, "ENEMY_DIE_FROM_PUNCH"},
	{3, "ENEMY_DIE_FROM_SHOT"},
	{4, "ENEMY_LIE_DEAD"},
	{5, "ENEMY_STAY_STILL"},
	{6, "ENEMY_FOLLOW_PRIMARY_ATTACK"},
	{7, "ENEMY_SECONDARY_ATTACK"},
	{8, "ENEMY_FOLLOW_SECONDARY_ATTACK"},
	{9, "ENEMY_JUMP"},
	{10, "ENEMY_INJURED"},
	{13, "ENEMY_SPECIAL"},
};
static std::map<uint32_t, const char*> debugStatesScenery = {
	{0, "SCENERY_NORMAL"},
	{1, "SCENERY_ATTACK"},
};

static char tmp[64];

void DarkForces::Component::SpriteAnimated::debugGUIinline(void)
{
	if (ImGui::TreeNode("SpriteAnimated")) {
		DarkForces::Component::Sprite::debugGUIinline();

#ifdef _DEBUG
		// display/hide the direction vector of the sprite
		bool b = m_debug;
		ImGui::Checkbox("Debug direction", &m_debug);
		if (b != m_debug) {
			directionVector();
		}
#endif
		dfComponentLogic* logic = dynamic_cast<dfComponentLogic*>(m_entity->findComponent(DF_COMPONENT_LOGIC));
		if (logic->logic() & DF_LOGIC_ENEMIES) {
			ImGui::Text("State: %s", debugStatesEnemies[static_cast<uint32_t>(m_state)]);
		}
		else if (logic->logic() & dfLogic::SCENERY) {
			ImGui::Text("State: %s", debugStatesScenery[static_cast<uint32_t>(m_state)]);
		}
		else {
			ImGui::Text("State: %d", static_cast<uint32_t>(m_state));
		}

		ImGui::Text("Frame: %d", static_cast<uint32_t>(m_frame));
		ImGui::Text("time last frame: %d", m_lastFrame);
		ImGui::Text("time current frame: %d", m_currentFrame);
		if (m_source) {
			ImGui::Text("frame rate: %d", m_source->framerate(m_state));
		}
		ImGui::TreePop();
	}
}

DarkForces::Component::SpriteAnimated::~SpriteAnimated()
{
#ifdef _DEBUG
	if (m_view) {
		g_gaWorld.remove2scene(m_view);
		delete m_view;
	}
#endif
}