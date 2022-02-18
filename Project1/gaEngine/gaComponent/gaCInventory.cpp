#include "gaCInventory.h"

#include <imgui.h>

//------------------------------------------------

void GameEngine::Component::Inventory::onAddItem(gaMessage* message)
{
	GameEngine::Item* item = static_cast<GameEngine::Item*>(message->m_extra);

	add(item);
}

void GameEngine::Component::Inventory::onDropItem(gaMessage* message)
{
	if (message->m_value < 0) {
		// drop all items
		for (auto& item : m_items) {
			item.second->dispatchMessage(message);
		}
	}
	else {
		//FIXME drop only one event
		printf("GameEngine::Component::Inventory::onDropItem: fixme\n");
	}
}

//------------------------------------------------

GameEngine::Component::Inventory::Inventory(void):
	gaComponent(gaComponent::Inventory)
{
}

void GameEngine::Component::Inventory::add(Item* item)
{
	//items are added only once
	if (m_items.count(item->name()) == 0 || m_items[item->name()] == nullptr) {
		// double link
		m_items[item->name()] = item;
		item->inventory(this);
	}
}

bool GameEngine::Component::Inventory::isPresent(const std::string& name)
{
	return m_items[name] != nullptr;
}

GameEngine::Item* GameEngine::Component::Inventory::get(const std::string& name)
{
	return m_items[name];
}

//------------------------------------------------

/**
 *
 */
void GameEngine::Component::Inventory::dispatchMessage(gaMessage* message)
{
	GameEngine::Item* item = nullptr;

	switch (message->m_action) {
	case gaMessage::Action::ADD_ITEM:
		onAddItem(message);
		break;
	case gaMessage::Action::DROP_ITEM:
		onDropItem(message);
		break;
	}
}

//------------------------------------------------

#ifdef _DEBUG
void GameEngine::Component::Inventory::debugGUIinline(void)
{
	if (ImGui::TreeNode("Inventory")) {
		for (auto& item : m_items) {
			if (item.second != nullptr) {
				if (ImGui::TreeNode(item.first.c_str())) {
					item.second->debugGUIinline();
					ImGui::TreePop();
				}
			}
		}
		ImGui::TreePop();
	}
}
#endif