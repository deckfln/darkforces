#pragma once

#include <glm/vec3.hpp>
#include "../../gaEngine/gaComponent.h"

enum {
	DF_KEY_NONE = 0,
	DF_KEY_RED = 1
};

class dfComponentActor : public gaComponent
{
	int32_t m_shield = 0;
	int32_t m_maxShield = 300;

	int32_t m_energy = 0;
	int32_t m_maxEnergy = 200;

	int32_t m_battery = 0;
	int32_t m_life = 0;

	uint32_t m_keys = DF_KEY_RED;

public:
	dfComponentActor(void);
	void addShield(int32_t value);
	void addEnergy(int32_t value);
	void hitBullet(int32_t value);							// hit by a bullet, reduce shield and life
	int keys(void) { return m_keys; };
	void fire(const glm::vec3& direction);				// handle the fire option
	void die(void);										// kill the actor

	void dispatchMessage(gaMessage* message) override;	// let an entity deal with a situation
	void debugGUIinline(void) override;					// display the component in the debugger

	~dfComponentActor();
};