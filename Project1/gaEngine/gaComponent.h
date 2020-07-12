#pragma once

#include "gaMessage.h"

enum {
	GA_COMPONENT_NONE = 0,
	GA_COMPONENT_MESH
};

class gaEntity;

class gaComponent
{
	int m_type = GA_COMPONENT_NONE;

protected:
	gaEntity* m_parent = nullptr;

public:
	gaComponent(int m_type);
	bool is(int type) { return type == m_type; };
	void parent(gaEntity* parent) { m_parent = parent; };
	virtual void dispatchMessage(gaMessage* message) {};// let a component deal with a situation
	~gaComponent();
};