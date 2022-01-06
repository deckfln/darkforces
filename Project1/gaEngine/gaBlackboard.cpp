#include "gaBlackboard.h"

#include "gaDebug.h"

void GameEngine::Blackboard::assign(const std::string& variable, const std::string& source)
{
	if (m_value[source] == nullptr) {
		return;
	}

	if (m_value[variable] == nullptr) {
		m_type[variable] = m_type[source];
		switch (m_type[source]) {
		case GameEngine::Variable::Type::BOOL:
			m_value[variable] = new bool;
			break;
		case GameEngine::Variable::Type::INT32:
			m_value[variable] = new int32_t;
			break;
		case GameEngine::Variable::Type::FLOAT:
			m_value[variable] = new float;
			break;
		case GameEngine::Variable::Type::STRING:
			m_value[variable] = new std::string;
			break;
		case GameEngine::Variable::Type::VEC3:
			m_value[variable] = new glm::vec3;
			break;
		}
	}

	if (m_type[variable] != m_type[source]) {
		gaDebugLog(1, "GameEngine::Blackboard::set", "incompatible type for " + variable);
	}

	switch (m_type[source]) {
	case GameEngine::Variable::Type::BOOL:
		*(static_cast<bool*>(m_value[variable])) = *(static_cast<bool*>(m_value[source]));
		break;
	case GameEngine::Variable::Type::INT32:
		*(static_cast<int32_t*>(m_value[variable])) = *(static_cast<int32_t*>(m_value[source]));
		break;
	case GameEngine::Variable::Type::FLOAT:
		*(static_cast<float*>(m_value[variable])) = *(static_cast<float*>(m_value[source]));
		break;
	case GameEngine::Variable::Type::STRING:
		*(static_cast<std::string*>(m_value[variable])) = *(static_cast<std::string*>(m_value[source]));
		break;
	case GameEngine::Variable::Type::VEC3:
		*(static_cast<glm::vec3*>(m_value[variable])) = *(static_cast<glm::vec3*>(m_value[source]));
		break;
	}
}
