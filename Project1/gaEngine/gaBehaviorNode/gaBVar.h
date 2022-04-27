#pragma once

#pragma once

#include <vector>
#include "../gaBehaviorNode.h"
#include "../gaVariable.h"
#include "../gaValue.h"

namespace GameEngine {
	namespace Behavior {
		class Var : public BehaviorNode {
		public:
			enum class Type {
				NONE,
				BOOL,
				INT32,
				FLOAT,
				VEC3,
				STRING,
				VAR,
				OBJECT
			};

			Var(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			// Behavior engine, GameEngine::BehaviorNode* used
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

#ifdef _DEBUG
			// debugger
			void debugGUInode(GameEngine::Component::BehaviorTree* tree) override;							// display the component in the debugger
#endif
		protected:
			Variable m_variable;
			Value m_value;
		};
	}
}