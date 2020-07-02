#pragma once

#include "../gaEngine/gaActor.h"

class dfActor : public gaActor
{
	int m_shield=0;
	int m_maxShield = 300;

	int m_energy = 0;
	int m_maxEnergy = 200;

	int m_battery = 0;
	int m_life = 0;
public:
	dfActor(const std::string& name, fwCylinder& bounding, float eyes, float ankle);
	void addShield(int value);
	void addEnergy(int value);
	~dfActor();
};