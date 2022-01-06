#pragma once

#include <string>
#include <map>
#include <glm/vec3.hpp>

#include "gaVariable.h"
#include "gaDebug.h"

namespace GameEngine {
	class Blackboard {
		std::map<std::string, GameEngine::Variable::Type> m_type;
		std::map<std::string, void*> m_value;
	public:
		template <typename T>
		void set(const std::string& key, const T& value, GameEngine::Variable::Type t);

		template <typename T>
		void set(const std::string& key, const T* value, GameEngine::Variable::Type t);

		void assign(const std::string& key, const std::string& source);

		template <typename T>
		T& get(const std::string& key, GameEngine::Variable::Type t);

		template <typename T>
		T* pGet(const std::string& key, GameEngine::Variable::Type t);
	};

	template<typename T>
	void GameEngine::Blackboard::set(const std::string& variable, const T& value, GameEngine::Variable::Type t)
	{
		if (m_value[variable] == nullptr) {
			m_value[variable] = new T;
			m_type[variable] = t;
		}

		if (m_type[variable] != t) {
			gaDebugLog(1, "GameEngine::Blackboard::set", "incompatible type for " + variable);
		}
		*(static_cast<T*>(m_value[variable])) = value;
	}

	template <typename T>
	void GameEngine::Blackboard::set(const std::string& variable, const T* pointer, GameEngine::Variable::Type t)
	{
		if (m_type.count(variable) == 0) {
			m_type[variable] = t;
		}
		
		if (m_type[variable] != t) {
			gaDebugLog(1, "GameEngine::Blackboard::set", "incompatible type for " + variable);
		}
		m_value[variable] = (void *)pointer;
		m_type[variable] = t;
	}

	template <typename T>
	T& GameEngine::Blackboard::get(const std::string& variable, GameEngine::Variable::Type t)
	{
		if (m_value[variable] == nullptr) {
			m_value[variable] = new T;
			m_type[variable] = t;
		}

		if (m_type[variable] != t) {
			gaDebugLog(1, "GameEngine::Blackboard::get", "incompatible type for " + variable);
		}

		return *(static_cast<T*>(m_value[variable]));
	}

	template <typename T>
	T* GameEngine::Blackboard::pGet(const std::string& variable, GameEngine::Variable::Type t)
	{
		if (m_type.count(variable) == 0) {
			m_type[variable] = t;
		}

		if (m_type[variable] != t) {
			gaDebugLog(1, "GameEngine::Blackboard::set", "incompatible type for " + variable);
		}
		return static_cast<T*>(m_value[variable]);
	}
}