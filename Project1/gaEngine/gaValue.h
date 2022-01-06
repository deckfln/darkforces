#pragma once

#include <tinyxml2.h>
#include "gaComponent/gaBehaviorTree.h"
#include "gaBehaviorNode/gaBVar.h"

namespace GameEngine {
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
		void set(tinyxml2::XMLElement* xmlVar);

		void get(bool& b, GameEngine::Component::BehaviorTree* tree) {
			if (m_type == Type::BOOL) {
				b = m_value;
			}
			else {
				b = tree->blackboard().get<bool>(m_svalue, GameEngine::Variable::Type::BOOL);
			}
		}
		void get(int32_t& b, GameEngine::Component::BehaviorTree* tree) {
			if (m_type == Type::INT32) {
				b = m_ivalue;
			}
			else {
				b = tree->blackboard().get<int32_t>(m_svalue, GameEngine::Variable::Type::INT32);
			}
		}
		void get(float& b, GameEngine::Component::BehaviorTree* tree) {
			if (m_type == Type::FLOAT) {
				b = m_fvalue;
			}
			else {
				b = tree->blackboard().get<float>(m_svalue, GameEngine::Variable::Type::FLOAT);
			}
		}
		void get(glm::vec3& b, GameEngine::Component::BehaviorTree* tree) {
			if (m_type == Type::VEC3) {
				b = m_v3value;
			}
			else {
				b = tree->blackboard().get<glm::vec3>(m_svalue, GameEngine::Variable::Type::VEC3);
			}
		}

	protected:
		Type m_type = Type::BOOL;					// variable type

		bool m_value = false;								// contents
		int32_t m_ivalue = 0;
		float m_fvalue = 0;
		glm::vec3 m_v3value = glm::vec3(0);
		std::string m_svalue;
		GameEngine::Component::BehaviorTree* m_tree = nullptr;
	};
}
