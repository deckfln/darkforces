#include "dfSprite.h"

#include <imgui.h>
#include "../../gaEngine/World.h"

#include "../dfSprites.h"
#include "../dfLevel.h"
#include "../dfModel.h"
#include "../dfModel/dfFME.h"

static uint32_t g_spriteID = 0;
static const char* g_className = "dfSprite";

/**
 * create from an image
 */
dfSprite::dfSprite(dfFME* fme, const glm::vec3& position, float ambient, uint32_t objectID):
	dfObject(fme, position, ambient, OBJECT_FME, objectID)
{
	m_className = g_className;
	addComponent(&m_componentLogic);
}

/**
 * create from a model
 */
dfSprite::dfSprite(dfModel* model, const glm::vec3& position, float ambient, int type, uint32_t objectID):
	dfObject(model, position, ambient, type, objectID)
{
	m_className = g_className;
	addComponent(&m_componentLogic);
}

/**
 * create from a model name
 */
dfSprite::dfSprite(const std::string& name, const glm::vec3& position, float ambient, int type) :
	dfObject((dfFME*)g_gaWorld.getModel(name), position, ambient, type, g_spriteID++)
{
	m_className = g_className;
	addComponent(&m_componentLogic);
}

dfSprite::dfSprite(flightRecorder::DarkForces::Sprite* record):
	dfObject(&record->object)
{
	g_spriteID++;
	m_className = g_className;
}

/**
 * Update the sprite buffers if the object is different
 */
bool dfSprite::updateSprite(glm::vec3* position, glm::vec4* texture, glm::vec3* direction)
{
	int updates = 0;

	if (m_dirtyAnimation) {
		texture->r = (float)m_source->id();
		texture->a = m_ambient;
		m_dirtyAnimation = false;
		updates++;
	}

	if (m_dirtyPosition) {
		*position = fwObject3D::position();
		m_dirtyPosition = false;
		updates++;
	}

	return updates > 0;
}

/**
 * trigger when inserted in a gaWorld
 *  add to the sprite manager
 */
void dfSprite::OnWorldInsert(void)
{
	dfSprites* manager = g_gaWorld.spritesManager();
	manager->add(this);

	dfObject::OnWorldInsert();
}

/**
 * trigger when from the gaWorld
 *  remove from the sprite manager
 */
void dfSprite::OnWorldRemove(void)
{
	dfSprites* manager = g_gaWorld.spritesManager();
	manager->remove(this);

	dfObject::OnWorldRemove();
}

/**
 * Deal with animation messages
 */
void dfSprite::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::MOVE:
		m_dirtyPosition = true;
		break;
	}
	dfObject::dispatchMessage(message);
}

/**
 * return a record of the entity state (for debug)
 */
uint32_t dfSprite::recordState(void* r)
{
	flightRecorder::DarkForces::Sprite* record = static_cast<flightRecorder::DarkForces::Sprite*>(r);
	dfObject::recordState(&record->object);

	record->object.entity.classID = flightRecorder::TYPE::DF_ENTITY_SPRITEANIMATED;
	record->object.entity.size = sizeof(flightRecorder::DarkForces::Sprite);

	return sizeof(flightRecorder::DarkForces::Sprite);
}

/**
 * reload an entity state from a record
 */
void dfSprite::loadState(void* r)
{
	flightRecorder::DarkForces::Sprite* record = (flightRecorder::DarkForces::Sprite*)r;
	dfObject::loadState((flightRecorder::Entity * )&record->object);
}

/**
 * Add dedicated component debug the entity
 */
void dfSprite::debugGUIChildClass(void)
{
	if (ImGui::TreeNode("dfSprite")) {
		ImGui::Text("slot: %d", m_slot);
		ImGui::TreePop();
	}
}

dfSprite::~dfSprite()
{
}
