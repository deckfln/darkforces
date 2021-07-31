#pragma once

#include <array>
#include "../config.h"
#include "../framework/controls/fwControlThirdPerson.h"

class fwCamera;
class gaActor;
class dfComponentActor;

class gaPlayer : public fwControlThirdPerson
{
	gaActor* m_entity = nullptr;
	dfComponentActor* m_actor = nullptr;

protected:
	bool checkKeys(time_t delta) override;
	void updatePlayer(time_t delta) override;

public:
	gaPlayer(fwCamera* camera, gaActor* actor, float phi);
	~gaPlayer();
};