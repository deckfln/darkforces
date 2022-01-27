#include "gaCInventory.h"

#include <imgui.h>

GameEngine::Component::Inventory::Inventory(void):
	gaComponent(gaComponent::Inventory)
{
}

void GameEngine::Component::Inventory::add(Item* item)
{
	m_items[item->name()] = item;
}

bool GameEngine::Component::Inventory::isPresent(const std::string& name)
{
	return m_items[name] != nullptr;
}

GameEngine::Item* GameEngine::Component::Inventory::get(const std::string& name)
{
	return m_items[name];
}

/**
 *
 */
void GameEngine::Component::Inventory::dispatchMessage(gaMessage* message)
{
	GameEngine::Item* item = nullptr;

	switch (message->m_action) {
	case gaMessage::Action::ADD_ITEM:
		add(static_cast<GameEngine::Item*>(message->m_extra));
		break;
	}
}

void GameEngine::Component::Inventory::debugGUIinline(void)
{
}
