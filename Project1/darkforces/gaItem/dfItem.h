#pragma once

#include "../../gaEngine/gaItem.h"

namespace DarkForces {
	class Object;

	class Item : public GameEngine::Item 
	{
	protected:
		bool m_droppable = true;				// item can be dropped when the entity dies
		uint32_t m_logic;						// DF logic of the item
		DarkForces::Object* m_object = nullptr;	// if the item is connected to a world entity

		void onDropItem(gaMessage* message);

	public:
		Item(const std::string& name, uint32_t logic);
		Item(const char* name, uint32_t logic);
		void Set(const char* name, uint32_t logic);
		void dispatchMessage(gaMessage* message) override;
		~Item(void);

	};
}