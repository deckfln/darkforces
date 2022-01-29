#pragma once

#include "../gaEngine/gaItem.h"

namespace DarkForces {
	class Item : public GameEngine::Item 
	{
		bool m_droppable = true;		// item can be dropped when the entity dies
		uint32_t m_logic;				// DF logic of the item

		void onDropItem(gaMessage* message);

	public:
		Item(const std::string& name, uint32_t logic);

		void dispatchMessage(gaMessage* message) override;
	};
}