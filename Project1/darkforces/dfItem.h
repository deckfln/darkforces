#pragma once

#include "../gaEngine/gaItem.h"

namespace DarkForces {
	class Object;

	class Item : public GameEngine::Item 
	{
		bool m_droppable = true;		// item can be dropped when the entity dies
		uint32_t m_logic;				// DF logic of the item
		DarkForces::Object* m_object = nullptr;

		void onDropItem(gaMessage* message);

	public:
		Item(const std::string& name, uint32_t logic);
		~Item(void);

		void dispatchMessage(gaMessage* message) override;
	};
}