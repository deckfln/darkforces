#pragma once

#include "../gaComponent.h"


namespace GameEngine {
	class BehaviorNode;

	namespace Component {
		class BehaviorTree : public gaComponent {
			BehaviorNode* m_root = nullptr;
			BehaviorNode* m_current = nullptr;
			bool m_instanciated = false;

		public:
			BehaviorTree(BehaviorNode* root);

			void dispatchMessage(gaMessage* message) override;		// let a component deal with a situation
		};
	}
}
