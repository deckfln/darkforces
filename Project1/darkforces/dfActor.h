#pragma once

#include "../gaEngine/gaActor.h"

enum {
	DF_KEY_NONE = 0,
	DF_KEY_RED = 1
};

class dfActor : public gaActor
{
	int m_shield=0;
	int m_maxShield = 300;

	int m_energy = 0;
	int m_maxEnergy = 200;

	int m_battery = 0;
	int m_life = 0;

	int m_keys = DF_KEY_RED;

public:
	dfActor(const std::string& name, fwCylinder& bounding, float eyes, float ankle);
	void addShield(int value);
	void addEnergy(int value);
	int keys(void) { return m_keys; };

	virtual void dispatchMessage(gaMessage* message);	// let an entity deal with a situation
	virtual void fire(const glm::vec3& direction);		// handle the fire option

	~dfActor();
};