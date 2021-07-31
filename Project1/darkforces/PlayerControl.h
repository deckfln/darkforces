#pragma once

#include "../gaEngine/gaPlayer.h"

class myDarkForces;

namespace DarkForces {
	class PlayerControl : public gaPlayer {
		bool m_keySpace = false;		// SPACE is pressed
		bool m_f5 = false;
		myDarkForces* m_parent = nullptr;

	protected:
		bool checkKeys(time_t delta) override;
	public:
		PlayerControl(fwCamera* camera, gaActor* actor, float phi);
		void setMyDarkForce(myDarkForces* parent) { m_parent = parent; };
		~PlayerControl();
	};
}