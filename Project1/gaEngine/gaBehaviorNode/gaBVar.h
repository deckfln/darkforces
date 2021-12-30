#pragma once

#pragma once

#include <vector>
#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Var : public BehaviorNode {
		public:
			enum class Type {
				NONE,
				BOOL,
				INT32,
				FLOAT,
				VEC3
			};

			Var(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			// Behavior engine, GameEngine::BehaviorNode* used
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			// debugger
			void debugGUInode(void) override;							// display the component in the debugger

		protected:
			std::string m_variable;						// list of angles to turn to
			Type m_type=Type::BOOL;							// variable type

			bool m_value;								// content for possible types
			int32_t m_ivalue;
			float m_fvalue;
			glm::vec3 m_v3value;
		};
	}
}