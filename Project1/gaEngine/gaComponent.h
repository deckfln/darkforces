#pragma once

#include "gaMessage.h"

class gaEntity;

class gaComponent
{
protected:
	gaEntity* m_entity = nullptr;

public:
	enum {
		NONE = 0,
		MESH,
		COLLIDER,
		CONTROLER
	};

	gaComponent(int m_type);
	bool is(int type) { return type == m_type; };
	void parent(gaEntity* parent) { m_entity = parent; };
	virtual void dispatchMessage(gaMessage* message) {};// let a component deal with a situation
	~gaComponent();

private:
	int m_type = NONE;
};