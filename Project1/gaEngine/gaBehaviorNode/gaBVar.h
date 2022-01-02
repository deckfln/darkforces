#pragma once

#pragma once

#include <vector>
#include "../gaBehaviorNode.h"
#include "../gaComponent/gaBehaviorTree.h"

namespace GameEngine {
	namespace Behavior {
		class Value {
		public:
			enum class Type {
				NONE,
				BOOL,
				INT32,
				FLOAT,
				VEC3,
				STRING,
				VAR
			};
			Value(void) {};
			void set(tinyxml2::XMLElement* xmlVar, GameEngine::Component::BehaviorTree *tree);

			void get(bool& b) {
				if (m_type == Type::BOOL) { 
					b = m_value; 
				}
				else {
					b = m_tree->blackboard<bool>(m_svalue);
				}
			}
			void get(int32_t& b) {
				if (m_type == Type::INT32) {
					b = m_ivalue;
				}
				else {
					b = m_tree->blackboard<int32_t>(m_svalue);
				}
			}
			void get(float& b) {
				if (m_type == Type::FLOAT) {
					b = m_fvalue;
				}
				else {
					b = m_tree->blackboard<float>(m_svalue);
				}
			}
			void get(glm::vec3& b) {
				if (m_type == Type::VEC3) {
					b = m_v3value;
				}
				else {
					b = m_tree->blackboard<glm::vec3>(m_svalue);
				}
			}

		protected:
			Type m_type = Type::BOOL;					// variable type

			bool m_value=false;								// contents
			int32_t m_ivalue=0;
			float m_fvalue=0;
			glm::vec3 m_v3value=glm::vec3(0);
			std::string m_svalue;
			GameEngine::Component::BehaviorTree* m_tree=nullptr;
		};

		class Var : public BehaviorNode {
		public:
			enum class Type {
				NONE,
				BOOL,
				INT32,
				FLOAT,
				VEC3,
				STRING,
				VAR
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
			std::string m_svalue;
		};
	}
}