#pragma once

#include <string>

#include "dfMessage.h"

class dfMessageClient
{
protected:
	std::string m_name;		// name of the object the trigger is bound to (sector or sector(wall)
	void addToBus(void);

public:
	dfMessageClient(void) {};
	dfMessageClient(std::string& name);
	virtual void dispatchMessage(dfMessage* message) {};
	std::string& name(void) { return m_name; };
	~dfMessageClient();
};