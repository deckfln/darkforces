#pragma once

#include <glm/vec3.hpp>
#include "../../gaEngine/gaComponent.h"

#include "../dfSector.h"
#include "../dfLevel.h"
#include "../dfConfig.h"

#include "../../config.h"

class dfComponentActor : public gaComponent
{
	int32_t m_shield = 100;
	int32_t m_maxShield = 300;

	int32_t m_energy = 0;
	int32_t m_maxEnergy = 200;

	int32_t m_battery = 0;
	int32_t m_life = 100;

	DarkForces::Keys m_keys = DarkForces::Keys::RED;

	dfSector* m_currentSector = nullptr;	// in what sector is the sector located
	dfLevel* m_level = nullptr;				// fast access to the loaded level

public:
	dfComponentActor(void);
	void addShield(int32_t value);
	void addEnergy(int32_t value);
	void hitBullet(int32_t value);							// hit by a bullet, reduce shield and life
	DarkForces::Keys keys(void) { return m_keys; };
	void fire(const glm::vec3& direction);				// handle the fire option
	void die(void);										// kill the actor
	inline dfSector* currentSector(void) { return m_currentSector; };
	inline void currentSector(dfSector* sector) { m_currentSector = sector; };
	inline void bind(dfLevel* level) { m_level = level; };
	void dispatchMessage(gaMessage* message) override;	// let an entity deal with a situation

	// flight recorder status
	inline uint32_t recordSize(void);					// size of the component record
	uint32_t recordState(void* record);					// save the component state in a record
	uint32_t loadState(void* record);					// reload a component state from a record

	//debugger
	void debugGUIinline(void) override;					// display the component in the debugger

	~dfComponentActor();
};