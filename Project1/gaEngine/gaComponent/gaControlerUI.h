#pragma once

#include <vector>
#include "../../framework/fwControl.h"

#include "../gaComponent.h"

namespace GameEngine {
	namespace Component {
		class ControllerUI : public gaComponent, public fwControl {
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

			ControllerUI(void);
			ControllerUI(const std::vector<KeyInfo>& keys);

			bool checkKeys(time_t delta);
			void _mouseButton(int action) override;				// convert mouse actions to messages

			// debugger
			void debugGUIinline(void) override;					// display the component in the debugger

		private:
			const std::vector<KeyInfo>	m_keys;					// keys to monitor
			bool m_left_mouse_down = false;						// real-time status of the button
		};
	}
}