#pragma once

#include <glm/vec3.hpp>
#include "../../gaEngine/gaComponent.h"

enum {
	DF_KEY_NONE = 0,
	DF_KEY_RED = 1
};

class dfComponentActor : public gaComponent
{
	int m_shield = 0;
	int m_maxShield = 300;

	int m_energy = 0;
	int m_maxEnergy = 200;

	int m_battery = 0;
	int m_life = 0;

	int m_keys = DF_KEY_RED;

public:
	dfComponentActor(void);
	void addShield(int value);
	void addEnergy(int value);
	void hitBullet(int value);							// hit by a bullet, reduce shield and life
	int keys(void) { return m_keys; };
	void fire(const glm::vec3& direction);				// handle the fire option
	void die(void);										// kill the actor

	virtual void dispatchMessage(gaMessage* message);	// let an entity deal with a situation

	~dfComponentActor();
};