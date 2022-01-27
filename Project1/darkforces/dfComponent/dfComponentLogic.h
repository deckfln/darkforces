#pragma once

#include <vector>
#include "../../gaEngine/gaComponent.h"
#include "../../gaEngine/gaComponent/gaCInventory.h"
#include "../dfConfig.h"

class gaMessage;
class dfComponentAI;

class dfComponentLogic : public gaComponent
{
	uint32_t m_logics=0;
	dfComponentAI* m_ai = nullptr;						// for logic objects with an AI
	uint32_t	m_value = 0;							// generic value to use in the logic
	std::vector<GameEngine::Item*> m_items;				// list of items delivered to the inventory

public:
	dfComponentLogic();
	void logic(uint32_t logic);								// add a logic
	inline uint32_t logic(void) {return m_logics;};			// retrieve the logic
	inline void setValue(uint32_t value) {	m_value = value; };

	void dispatchMessage(gaMessage* message) override;	// deal with messages

	void debugGUIinline(void) override;					// display the component in the debugger

	~dfComponentLogic();
};