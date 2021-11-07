#include "dfCSprite.h"

#include <imgui.h>
#include "../../gaEngine/World.h"

#include "../dfComponent.h"

#include "../dfSprites.h"
#include "../dfLevel.h"
#include "../dfModel.h"
#include "../dfModel/dfFME.h"

static uint32_t g_spriteID = 0;

/**
 * create from an image
 */
DarkForces::Component::Sprite::Sprite(dfFME* fme, float ambient) :
	gaComponent(DF_COMPONENT_SPRITE),
	m_source(fme),
	m_ambient(ambient)
{
}

/**
 * create from a model
 */
DarkForces::Component::Sprite::Sprite(dfModel* model, float ambient) :
	gaComponent(DF_COMPONENT_SPRITE),
	m_source(model),
	m_ambient(ambient)
{
}

DarkForces::Component::Sprite::Sprite(const std::string& model, float ambient):
	gaComponent(DF_COMPONENT_SPRITE),
	m_source(static_cast<dfModel*>(g_gaWorld.getModel(model))),
	m_ambient(ambient)
{
}

DarkForces::Component::Sprite::Sprite(void) :
	gaComponent(DF_COMPONENT_SPRITE)
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
		texture->a = m_ambient;
		m_dirtyAnimation = false;
		updates++;
	}

	if (m_dirtyPosition) {
		*position = m_entity->position();
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
	dfSprites* manager = g_gaWorld.spritesManager();
	manager->add(this);
}

/**
 * trigger when from the gaWorld
 *  remove from the sprite manager
 */
void DarkForces::Component::Sprite::onWorldRemove(void)
{
	dfSprites* manager = g_gaWorld.spritesManager();
	manager->remove(this);
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
		m_dirtyPosition = true;
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
