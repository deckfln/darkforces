#pragma once

#include "../framework/controls/fwControlThirdPerson.h"

class fwCamera;
class gaActor;

class gaPlayer : public fwControlThirdPerson
{
	gaActor* m_actor = nullptr;

	virtual bool checkKeys(time_t delta);
	virtual void updatePlayer(time_t delta);

public:
	gaPlayer(fwCamera* camera, gaActor* actor, float phi);
	~gaPlayer();
};