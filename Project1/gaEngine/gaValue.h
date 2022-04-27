#pragma once

#include <tinyxml2.h>
#include <string>
#include <glm/vec3.hpp>

namespace GameEngine {
	namespace Component {
		class BehaviorTree;
	};
	class Variable;

	class Value {
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
		Value(void) {};
		void set(tinyxml2::XMLElement* xmlVar);
		bool create(tinyxml2::XMLElement* xmlVar);
		inline const std::string& var(void) { return m_svalue; };
		inline Type type(void) { return m_type; };

		void get(bool& b, GameEngine::Component::BehaviorTree* tree);
		void get(int32_t& b, GameEngine::Component::BehaviorTree* tree);
		void get(float& b, GameEngine::Component::BehaviorTree* tree);
		void get(glm::vec3& b, GameEngine::Component::BehaviorTree* tree);

		bool& getb(GameEngine::Component::BehaviorTree* tree) ;
		int32_t& geti(GameEngine::Component::BehaviorTree* tree) ;
		float& getf(GameEngine::Component::BehaviorTree* tree) ;
		glm::vec3& getv3(GameEngine::Component::BehaviorTree* tree) ;
		std::string& gets(GameEngine::Component::BehaviorTree* tree) ;

#ifdef _DEBUG
		// debugger
		const char* debug(void);
#endif

	protected:
		Type m_type = Type::NONE;							// value type

		bool m_value = false;								// contents
		int32_t m_ivalue = 0;
		float m_fvalue = 0;
		glm::vec3 m_v3value = glm::vec3(0);
		std::string m_svalue;
	};
}
