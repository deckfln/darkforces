#pragma once

#include "../../gaEngine/gaComponent.h"
#include "../dfConfig.h"

class gaMessage;
class dfComponentAI;

class dfComponentLogic : public gaComponent
{
	uint32_t m_logics=0;
	dfComponentAI* m_ai = nullptr;						// for logic objects with an AI

public:
	dfComponentLogic();
	void logic(uint32_t logic);								// add a logic
	void dispatchMessage(gaMessage* message) override;	// deal with messages
	void debugGUIinline(void) override;					// display the component in the debugger

	~dfComponentLogic();
};