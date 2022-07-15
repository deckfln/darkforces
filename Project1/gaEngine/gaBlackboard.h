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
		// access variables in the blackboard
		template <typename T>
		void set(const std::string& key, const T& value, GameEngine::Variable::Type t);

		template <typename T>
		void set(const std::string& key, const T* value, GameEngine::Variable::Type t);

		void assign(const std::string& key, const std::string& source);

		template <typename T>
		T& get(const std::string& key, GameEngine::Variable::Type t);

		template <typename T>
		T* pGet(const std::string& key, GameEngine::Variable::Type t);

		template <typename T>
		void pSet(const std::string& key, T* ptr);

		// flight recorder status
		uint32_t recordSize(void);					// size of the component record
		uint32_t recordState(void* record);					// save the component state in a record
		uint32_t loadState(void* record);					// reload a component state from a record
	};

	template<typename T>
	void GameEngine::Blackboard::set(const std::string& variable, const T& value, GameEngine::Variable::Type t)
	{
		if (m_value[variable] == nullptr) {
			m_value[variable] = new T;
			m_type[variable] = t;
		}

#ifdef _DEBUG
		if (m_type[variable] != t) {
			gaDebugLog(1, "GameEngine::Blackboard::set", "incompatible type for " + variable);
			__debugbreak();
		}
#endif
		*(static_cast<T*>(m_value[variable])) = value;
	}

	template <typename T>
	void GameEngine::Blackboard::set(const std::string& variable, const T* pointer, GameEngine::Variable::Type t)
	{
		if (m_type.count(variable) == 0) {
			m_type[variable] = t;
		}
		
#ifdef _DEBUG
		if (m_type[variable] != t) {
			gaDebugLog(1, "GameEngine::Blackboard::set", "incompatible type for " + variable);
			__debugbreak();
		}
#endif
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

#ifdef _DEBUG
		if (m_type[variable] != t) {
			gaDebugLog(1, "GameEngine::Blackboard::get", "incompatible type for " + variable);
			__debugbreak();
		}
#endif

		return *(static_cast<T*>(m_value[variable]));
	}

	template <typename T>
	T* GameEngine::Blackboard::pGet(const std::string& variable, GameEngine::Variable::Type t)
	{
		if (m_type.count(variable) == 0) {
			m_type[variable] = t;
		}

#ifdef _DEBUG
		if (m_type[variable] != t) {
			gaDebugLog(1, "GameEngine::Blackboard::set", "incompatible type for " + variable);
			__debugbreak();
		}
#endif
		return static_cast<T*>(m_value[variable]);
	}

	template <typename T>
	void GameEngine::Blackboard::pSet(const std::string& variable, T* ptr)
	{
		if (m_type.count(variable) == 0) {
			m_type[variable] = GameEngine::Variable::Type::PTR;
		}

#ifdef _DEBUG
		if (m_type[variable] != GameEngine::Variable::Type::PTR) {
			gaDebugLog(1, "GameEngine::Blackboard::pSet", "incompatible type for " + variable);
		}
#endif
		m_value[variable] = ptr;
	}

}