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

		void set(GameEngine::Component::BehaviorTree* tree, void* ptr);	// set on the blackboard from a PTR
		void set(GameEngine::Component::BehaviorTree* tree, bool b);	// set on the blackboard from an BOOL
		void set(GameEngine::Component::BehaviorTree* tree, int32_t i);	// set on the blackboard from an int32
		void set(GameEngine::Component::BehaviorTree* tree, float f);	// set on the blackboard from an int32
		void set(GameEngine::Component::BehaviorTree* tree, const glm::vec3& i);	// set on the blackboard from an VEC3
		void set(GameEngine::Component::BehaviorTree* tree, const std::string& i);	// set on the blackboard from an STRING

		// compare
		void set(GameEngine::Component::BehaviorTree* tree, GameEngine::Value& v);			// set on the blackboard from a value
		bool equal(GameEngine::Component::BehaviorTree* tree, GameEngine::Value& v);

		const char* debug(void);

	protected:
		std::string m_name;
		Type m_type=Type::NONE;
	};
}