#pragma once

#include "../../framework/fwHUD.h"
#include "../../framework/fwHUDelement.h"
#include "../../gaEngine/gaEntity.h"
#include "../../framework/fwHUD.h"
#include "../../gaEngine/gaComponent/gaControlerUI.h"
#include "../../gaEngine/gaUI.h"

#include "../dfComponent/dfCPDA.h"

namespace DarkForces {
	namespace Prefab {
		class PDA : public gaEntity {
			GameEngine::Component::ControllerUI m_controler;	// basic controler for an UI
			Component::PDA m_pda;
		public:
			PDA(void);
			GameEngine::UI* ui(void);
		};
	}
}