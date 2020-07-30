#pragma once

#include "../../gaEngine/gaComponent.h"

class gaMessage;
class dfComponentAI;

class dfComponentLogic : public gaComponent
{
	int m_logics=0;
	dfComponentAI* m_ai = nullptr;						// for logic objects with an AI

public:
	dfComponentLogic();
	void logic(int logic);								// add a logic
	void dispatchMessage(gaMessage* message) override;	// deal with messages
	~dfComponentLogic();
};