#include "dfSpriteAnimated.h"

#include <glm/trigonometric.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

#include <imgui.h>

#include "../../config.h"

#include "../../framework/geometries/fwGeometryCylinder.h"

#include "../../gaEngine/World.h"

#include "../dfModel/dfWAX.h"
#include "../dfLevel.h"
#include "../dfComponent/dfComponentLogic.h"

static uint32_t g_animatedSpriteID = 0;
static const char* g_className = "dfSpriteAnimated";

static glm::vec4 g_red(1.0, 1.0, 0.0, 1.0);
static fwMaterialBasic g_basic(&g_red);
static fwGeometryCylinder* g_view = nullptr;

/**
 * create a sprite from a pointer to a model
 */
dfSpriteAnimated::dfSpriteAnimated(dfWAX* wax, const glm::vec3& position, float ambient, uint32_t objectID):
	dfSprite(wax, position, ambient, OBJECT_WAX, objectID)
{
	if (g_view == nullptr) {
		g_basic.makeStatic();
		g_view = new fwGeometryCylinder(0.01f, 0.15f, 4, 1);
		g_view->makeStatic();
	}

#ifdef _DEBUG
	// add the mesh component to the entity
	m_view = new fwMesh(g_view, &g_basic);
 	m_view->translate(get_position() + glm::vec3(0.0f, 0.5f, 0.0f));
	g_gaWorld.add2scene(m_view);
#endif

	gaEntity::updateWorldAABB();
	m_className = g_className;
}

/**
 * create a sprite from a model name, the real model is extracted from the world
 */
dfSpriteAnimated::dfSpriteAnimated(const std::string& model, const glm::vec3& position, float ambient):
	dfSprite((dfWAX*)g_gaWorld.getModel(model), position, ambient, OBJECT_WAX, g_animatedSpriteID++)
{
	gaEntity::updateWorldAABB();
	m_className = g_className;
}

dfSpriteAnimated::dfSpriteAnimated(flightRecorder::DarkForces::SpriteAnimated* record) :
	dfSprite(&record->sprite)
{
	loadState((flightRecorder::Entity *)record);
	m_className = g_className;
}

/**
 * Change the state of an object
 */
void dfSpriteAnimated::state(dfState state)
{
	m_state = state;
	modelAABB(((dfWAX*)m_source)->bounding(m_state));
	dfSprite::updateWorldAABB();
	m_lastFrame = m_currentFrame = 0;
	m_frame = 0;
	m_dirtyAnimation = true;

	if (m_logics & DF_LOGIC_ENEMIES) {
		// trigger the animation, unless the object is static or has no animation
		if (m_state != dfState::ENEMY_LIE_DEAD &&
			m_state != dfState::ENEMY_STAY_STILL &&
			m_source->nbFrames(m_state) > 1)
		{
			g_gaWorld.sendMessageDelayed(m_name, m_name, gaMessage::TIMER, 0, nullptr);
		}
	}
	else {
		// trigger an animation loop if there is actually an animation loop
		if ((m_logics & dfLogic::ANIM) && m_source->nbFrames(m_state) > 1) {
			g_gaWorld.sendMessageDelayed(m_name, m_name, gaMessage::TIMER, 0, nullptr);
		}
	}
}

bool dfSpriteAnimated::updateSprite(glm::vec3* position, glm::vec4* texture, glm::vec3* direction)
{
	if (m_dirtyPosition) {
		*direction = m_direction;
	}

	if (m_dirtyAnimation) {
		texture->g = (float)m_state;
		texture->b = (float)m_frame;
	}

	return dfSprite::updateSprite(position, texture, direction);
}

/**
 * Create a direction vector ased on pch, yaw, rol
 */
void dfSpriteAnimated::rotation(const glm::vec3& rotation)
{
	// YAW = value in degrees where 0 is at the "top of the screen when you look at the map". The value increases clockwise
	float yaw = glm::radians(rotation.y);
	m_direction.x = sin(yaw);	// in level space
	m_direction.y = 0;
	m_direction.z = -cos(yaw);

#ifdef _DEBUG
	float a = atan2(m_direction.x, m_direction.z);// +M_PI / 2.0f;
	glm::vec3 xr(0, a, 0);
	m_view->rotate(xr);
#endif
}

/**
 * Animate the object frame
 */
bool dfSpriteAnimated::update(time_t t)
{
	if (!(m_logics & dfLogic::ANIM)) {
		return false;
	}

	m_currentFrame += t;

	uint32_t nbFrames = m_source->nbFrames(m_state);
	if (nbFrames <= 1) {
		return false;	// static objects like FME are not updated, 
						// objects with only 1 frame in the animation loop are also not updated
	}

	uint32_t frameRate = m_source->framerate(m_state);
	time_t frameTime = 1000 / frameRate; // time of one frame in milliseconds

	time_t delta = m_currentFrame - m_lastFrame;
	if (delta >= frameTime) {
		m_frame = m_source->nextFrame(m_state, m_frame);

		if (m_frame == -1) {
			return false;	// reached end of animation loop
		}

		m_lastFrame = (m_currentFrame / frameTime) * frameTime;
		m_dirtyAnimation = true;
	}

	return true;	// continue animation loop
}

/**
 * Deal with animation messages
 */
void dfSpriteAnimated::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MSG_STATE:
		state((dfState)message->m_value);
		break;

	case gaMessage::TIMER:
		if (update(message->m_delta)) {
			// continue the animation loop
			g_gaWorld.sendMessageDelayed(m_name, m_name, gaMessage::TIMER, 0, nullptr);
		}
		else {
			sendInternalMessage(DF_MESSAGE_END_LOOP, (int)m_state);
		}
		break;

	case DF_MESSAGE_END_LOOP:
		// go for next animation if the animation loop ?
		if (m_loopAnimation && m_source->nbFrames(m_state) > 1) {
			// restart the counters
			m_lastFrame = m_currentFrame = 0;
			m_frame = 0;
			m_dirtyAnimation = true;
			// and reboot the timer for the next frame
			g_gaWorld.sendMessageDelayed(m_name, m_name, gaMessage::TIMER, 0, nullptr);
		}
		break;

	case gaMessage::LOOK_AT:
		if (message->m_extra == nullptr) {
			m_direction = glm::normalize(message->m_v3value);
			m_dirtyPosition = true;
#ifdef _DEBUG
			float a = atan2(m_direction.x, m_direction.z);// +M_PI / 2.0f;
			glm::vec3 xr(0, a, 0);

			m_view->rotate(xr);
#endif
		}
		break;

	case gaMessage::START_MOVE:
		state(dfState::ENEMY_MOVE);
		break;

	case gaMessage::END_MOVE:
		state(dfState::ENEMY_STAY_STILL);
		break;

	case gaMessage::MOVE:
		m_view->translate(get_position() + glm::vec3(0.0f, 0.5f, 0.0f));
		break;
	}
	dfSprite::dispatchMessage(message);
}

/**
 * return a record of the entity state (for debug)
 */
uint32_t dfSpriteAnimated::recordState(void* r)
{
	dfSprite::recordState(r);
	flightRecorder::DarkForces::SpriteAnimated* record = static_cast<flightRecorder::DarkForces::SpriteAnimated*>(r);

	record->sprite.object.entity.classID = flightRecorder::TYPE::DF_ENTITY_SPRITEANIMATED;
	record->sprite.object.entity.size = sizeof(flightRecorder::DarkForces::SpriteAnimated);

	record->state = static_cast<uint32_t>(m_state);			// state of the object for WAX, unused for others
	record->frame = m_frame;			// current frame to display based on frameSpeed
	record->direction = m_direction;	// direction the object is looking to
	record->lastFrame = m_lastFrame;		// time of the last animation frame
	record->currentFrame = m_currentFrame;	// time of the current animation frame

	return record->sprite.object.entity.size;
}

/**
 * reload an entity state from a record
 */
void dfSpriteAnimated::loadState(void* r)
{
	dfSprite::loadState(r);
	flightRecorder::DarkForces::SpriteAnimated* record = static_cast<flightRecorder::DarkForces::SpriteAnimated*>((void *)r);
	m_state = static_cast<dfState>(record->state);			// state of the object for WAX, unused for others
	m_frame = record->frame;			// current frame to display based on frameSpeed
	m_direction = record->direction;	// direction the object is looking to
	m_lastFrame = record->lastFrame;		// time of the last animation frame
	m_currentFrame = record->currentFrame;	// time of the current animation frame

	m_dirtyAnimation = true;				// force refresh of the sprite
}

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

/**
 * Add dedicated component debug the entity
 */
void dfSpriteAnimated::debugGUIChildClass(void)
{
	dfSprite::debugGUIChildClass();

	if (ImGui::TreeNode("dfSpriteAnimated")) {
		if (m_logics & DF_LOGIC_ENEMIES) {
			ImGui::Text("State: %s", debugStatesEnemies[static_cast<uint32_t>(m_state)]);
		}
		else if (m_logics & dfLogic::SCENERY) {
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

dfSpriteAnimated::~dfSpriteAnimated()
{
}