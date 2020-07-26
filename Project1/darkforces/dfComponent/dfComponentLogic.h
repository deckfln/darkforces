#pragma once

#include "../../gaEngine/gaComponent.h"

class gaMessage;

class dfComponentLogic : public gaComponent
{
	int m_logics=0;
public:
	dfComponentLogic();
	void logic(int logic);								// add a logic
	void dispatchMessage(gaMessage* message) override;	// deal with messages
	~dfComponentLogic();
};