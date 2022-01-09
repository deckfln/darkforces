#pragma once

#include <string>
#include <tinyxml2.h>
#include <map>
#include <glm/vec3.hpp>
#include "gaValue.h"

namespace GameEngine {
	namespace Component {
		class BehaviorTree;
	}
	class Variable {
	public:
		enum class Type {
			NONE,
			BOOL,
			INT32,
			FLOAT,
			VEC3,
			STRING,
			VAR,
			OBJECT,
			PTR
		};
		Variable() {}
		bool create(tinyxml2::XMLElement* element);

		bool& getb(GameEngine::Component::BehaviorTree* tree);
		int32_t& geti(GameEngine::Component::BehaviorTree* tree);
		float& getf(GameEngine::Component::BehaviorTree* tree);
		glm::vec3& getv3(GameEngine::Component::BehaviorTree* tree);
		std::string& gets(GameEngine::Component::BehaviorTree* tree);
		void* getp(GameEngine::Component::BehaviorTree* tree);
		void get(GameEngine::Component::BehaviorTree* tree, const std::string& var);

		inline const std::string& var(void) { return m_var; };

		void set(GameEngine::Component::BehaviorTree* tree, void* ptr);	// set on the blackboard from a PTR
		void set(GameEngine::Component::BehaviorTree* tree, int32_t i);	// set on the blackboard from an int32
		void set(GameEngine::Component::BehaviorTree* tree, float f);	// set on the blackboard from an int32
		void set(GameEngine::Component::BehaviorTree* tree, const glm::vec3& i);	// set on the blackboard from an VEC3
		void set(GameEngine::Component::BehaviorTree* tree, const std::string& i);	// set on the blackboard from an STRING
		void set(GameEngine::Component::BehaviorTree* tree);			// set on the blackboard from predefined value

		// set the predefined value
		inline void set(bool b) { m_value = b; };
		inline void set(int32_t i) { m_ivalue = i; };
		inline void set(float f) { m_fvalue = f; };
		inline void set(const glm::vec3& v) { m_v3value = v; };
		inline void set(const std::string& v) { m_svalue = v; };

		// compare
		void set(GameEngine::Component::BehaviorTree* tree, GameEngine::Value& v);			// set on the blackboard from a value
		bool equal(GameEngine::Component::BehaviorTree* tree, GameEngine::Value& v);

		const char* debug(void);

	protected:
		std::string m_name;
		Type m_type=Type::NONE;

		// init value
		bool m_value=false;								// content for possible types
		int32_t m_ivalue=0;
		float m_fvalue=0.0f;
		glm::vec3 m_v3value=glm::vec3(0);
		std::string m_svalue;
		std::string m_var;
	};
}