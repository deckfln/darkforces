#include "dfCSprite.h"

#include <imgui.h>
#include "../../gaEngine/World.h"

#include "../dfComponent.h"

#include "../dfLevel.h"
#include "../dfModel.h"
#include "../dfModel/dfFME.h"
#include "../dfPlugin/dfSprites.h"

static uint32_t g_spriteID = 0;

/**
 * create from an image
 */
DarkForces::Component::Sprite::Sprite(dfFME* fme, float ambient) :
	gaComponent(DF_COMPONENT_SPRITE, "dfSprite"),
	m_source(fme),
	m_ambient(ambient)
{
}

/**
 * create from a model
 */
DarkForces::Component::Sprite::Sprite(dfModel* model, float ambient) :
	gaComponent(DF_COMPONENT_SPRITE, "dfSprite"),
	m_source(model),
	m_ambient(ambient)
{
}

DarkForces::Component::Sprite::Sprite(const std::string& model, float ambient):
	gaComponent(DF_COMPONENT_SPRITE, "dfSprite"),
	m_source(static_cast<dfModel*>(g_gaWorld.getModel(model))),
	m_ambient(ambient)
{
}

DarkForces::Component::Sprite::Sprite(void) :
	gaComponent(DF_COMPONENT_SPRITE, "dfSprite")
{
}

/**
 * set parameters after creation
 */
void DarkForces::Component::Sprite::set(const std::string& model, float ambient)
{
	m_source = static_cast<dfModel*>(g_gaWorld.getModel(model));
	m_ambient = ambient;
}

/**
 * Update the sprite buffers if the object is different
 */
bool DarkForces::Component::Sprite::update(glm::vec3* position, glm::vec4* texture, glm::vec3* direction)
{
	int updates = 0;

	if (m_dirtyAnimation) {
		texture->r = (float)m_source->id();
		m_dirtyAnimation = false;
		updates++;
	}

	if (m_dirtyPosition) {
		*position = m_entity->position();
		texture->a = m_ambient;
		m_dirtyPosition = false;
		updates++;
	}

	return updates > 0;
}

/**
 * trigger when inserted in a gaWorld
 *  add to the sprite manager
 */
void DarkForces::Component::Sprite::onWorldInsert(void)
{
	g_dfSpritesEngine.add(this);
}

/**
 * trigger when from the gaWorld
 *  remove from the sprite manager
 */
void DarkForces::Component::Sprite::onWorldRemove(void)
{
	g_dfSpritesEngine.remove(this);
}

/**
 * change lighting when object moves
 */
void DarkForces::Component::Sprite::onMove(gaMessage* message)
{
	dfSector* currentSector = nullptr;
	const glm::vec3& p = message->m_v3value;

	currentSector = static_cast<dfLevel*>(g_gaLevel)->findSector(m_entity->position());
	if (currentSector) {
		m_ambient = currentSector->ambient() / 32.0;
	}
	m_dirtyPosition = true;
}

/**
 * Deal with animation messages
 */
void DarkForces::Component::Sprite::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::WORLD_INSERT:
		onWorldInsert();
		break;

	case gaMessage::WORLD_REMOVE:
		onWorldRemove();
		break;

	case gaMessage::MOVE:
		onMove(message);
		break;
	}
	gaComponent::dispatchMessage(message);
}

void DarkForces::Component::Sprite::debugGUIinline(void)
{
	if (ImGui::TreeNode("Sprite")) {
		ImGui::Text("Model: %s", m_source->name().c_str());
		ImGui::Text("ID: %d", m_slot);
		ImGui::TreePop();
	}
}
