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

#ifdef DEBUG
	bool m_replay = true;
	bool m_record = false;

	std::array<glm::vec3, 3000> m_recorder;
	int m_recorder_start = 0;
	int m_recorder_end = 0;
	int m_recorder_len = 0;
#endif

	virtual bool checkKeys(time_t delta);
	virtual void updatePlayer(time_t delta);

public:
	gaPlayer(fwCamera* camera, gaActor* actor, float phi);
	~gaPlayer();
};