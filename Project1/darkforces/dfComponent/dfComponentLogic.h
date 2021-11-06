#pragma once

#include "../../gaEngine/gaComponent.h"
#include "../dfConfig.h"

class gaMessage;
class dfComponentAI;

class dfComponentLogic : public gaComponent
{
	uint32_t m_logics=0;
	dfComponentAI* m_ai = nullptr;						// for logic objects with an AI
	uint32_t	m_value = 0;							// generic value to use in the logic

public:
	dfComponentLogic();
	void logic(uint32_t logic);								// add a logic
	inline void setValue(uint32_t value) {	m_value = value; };
	void dispatchMessage(gaMessage* message) override;	// deal with messages

	void debugGUIinline(void) override;					// display the component in the debugger

	~dfComponentLogic();
};