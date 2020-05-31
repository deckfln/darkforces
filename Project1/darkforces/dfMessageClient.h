#pragma once

#include <string>

#include "dfMessage.h"

class dfMessageClient
{
	dfMessageClient* m_next = nullptr;	// chain multiple client with the same name

protected:
	std::string m_name;		// name of the object the trigger is bound to (sector or sector(wall)
	void addToBus(void);

public:
	dfMessageClient(void) {};
	dfMessageClient(std::string& name);
	virtual void dispatchMessage(dfMessage* message);
	std::string& name(void) { return m_name; };
	void chain(dfMessageClient* next);
	~dfMessageClient();
};