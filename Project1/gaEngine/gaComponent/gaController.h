#pragma once
#include <glm/vec3.hpp>
#include <vector>
#include "../../framework/controls/fwControlThirdPerson.h"
#include "../../framework/fwCamera.h"

#include "../gaComponent.h"

namespace GameEngine {
	namespace Component {
		class Controller : public gaComponent, public fwControlThirdPerson {
		public:
			struct KeyInfo {
				enum class Msg {
					onChange,
					onPress,
					onPressDown,
					onPressUp
				};
				uint32_t m_key;
				Msg m_msh;
			};

			Controller(fwCamera*,
				const glm::vec3& position,
				float height,
				float phi,
				float radius,
				const std::vector<KeyInfo>& keys);
			~Controller();

			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void debugGUIinline(void) override;					// display the component in the debugger

			bool checkKeys(time_t delta) override;
			void updatePlayer(time_t delta) override;

		private:
			glm::vec3 m_oldEye;
			glm::vec3 m_oldLookAt;
			const std::vector<KeyInfo>	m_keys;						// keys to monitor

		};
	}
}
