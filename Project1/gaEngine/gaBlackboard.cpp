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

//---------------------------------------------------

/**
 * dynamic size of the blackboard
 */
uint32_t GameEngine::Blackboard::recordSize(void)
{
	uint32_t l = sizeof(uint32_t);

	for (auto& value: m_value) {
		switch (m_type[value.first])
		{
		case GameEngine::Variable::Type::BOOL:
			l += sizeof(bool);
			l += value.first.length() + 1;
			break;
		case GameEngine::Variable::Type::INT32:
			l += sizeof(int32_t);
			l += value.first.length() + 1;
			break;
		case GameEngine::Variable::Type::FLOAT:
			l += sizeof(float);
			l += value.first.length() + 1;
			break;
		case GameEngine::Variable::Type::STRING:
			l += static_cast<const std::string*>(value.second)->length();
			l += value.first.length() + 1;
			break;
		case GameEngine::Variable::Type::VEC3:
			l += sizeof(glm::vec3);
			l += value.first.length() + 1;
			break;

		default:
			break;
		}
	}
	return l;
}

struct headerBool {
	uint32_t type;
	uint32_t name_len;
	bool b;
	char name[1];
};

struct headerInt32 {
	uint32_t type;
	uint32_t name_len;
	int32_t i;
	char name[1];
};

struct headerFloat {
	uint32_t type;
	uint32_t name_len;
	float f;
	char name[1];
};

struct headerVec3 {
	uint32_t type;
	uint32_t name_len;
	glm::vec3 v3;
	char name[1];
};

struct headerString {
	uint32_t type;
	uint32_t name_len;
	uint32_t str_len;
	char name[1];
};

/**
 *
 */
uint32_t GameEngine::Blackboard::recordState(void* record)
{
	uint32_t l = sizeof(uint32_t);
	char* p = (char*)record;
	uint32_t* nbVariable = (uint32_t*)p;
	*nbVariable = 0;
	p += sizeof(uint32_t);

	for (auto& value : m_value) {
		switch (m_type[value.first])
		{
		case GameEngine::Variable::Type::BOOL: {
			struct headerBool* pHeader = (struct headerBool*)p;
			pHeader->type = 0;
			pHeader->name_len = value.first.length() + 1;
			pHeader->b = *(static_cast<bool*>(value.second));
			strcpy_s(pHeader->name, 255, value.first.c_str());
			p += (sizeof(headerBool) + pHeader->name_len);
			(*nbVariable)++;
			break; }

		case GameEngine::Variable::Type::INT32: {
			struct headerInt32* pHeader = (struct headerInt32*)p;
			pHeader->type = 1;
			pHeader->name_len = value.first.length() + 1;
			pHeader->i = *(static_cast<int32_t*>(value.second));
			strcpy_s(pHeader->name, 255, value.first.c_str());
			p += (sizeof(headerInt32) + pHeader->name_len);
			(*nbVariable)++;
			break; }

		case GameEngine::Variable::Type::FLOAT: {
			struct headerFloat* pHeader = (struct headerFloat*)p;
			pHeader->type = 2;
			pHeader->name_len = value.first.length() + 1;
			pHeader->f = *(static_cast<float*>(value.second));
			strcpy_s(pHeader->name, 255, value.first.c_str());
			p += (sizeof(headerFloat) + pHeader->name_len);
			(*nbVariable)++;
			break;	}

		case GameEngine::Variable::Type::STRING: {
			struct headerString* pHeader = (struct headerString*)p;
			std::string* str = static_cast<std::string*>(value.second);
			pHeader->type = 3;
			pHeader->name_len = value.first.length() + 1;
			pHeader->str_len = str->length() + 1;
			strcpy_s(pHeader->name, 255, value.first.c_str());
			//strcpy_s(&pHeader->name[pHeader->name_len], str->c_str());
			p += (sizeof(headerString) + pHeader->name_len + pHeader->str_len);
			(*nbVariable)++;
			break;
		}

		case GameEngine::Variable::Type::VEC3: {
			struct headerVec3* pHeader = (struct headerVec3*)p;
			pHeader->type = 4;
			pHeader->name_len = value.first.length() + 1;
			pHeader->v3 = *(static_cast<glm::vec3*>(value.second));
			strcpy_s(pHeader->name, 255, value.first.c_str());
			p += (sizeof(headerVec3) + pHeader->name_len);
			(*nbVariable)++;
			break; }

		default:
			break;
		}
	}
	return p - (char *)record;
}

/**
 *
 */
uint32_t GameEngine::Blackboard::loadState(void* record)
{
	char* p = (char*)record;
	uint32_t* nbVariable = (uint32_t*)p;
	p += sizeof(uint32_t);

	for (size_t i = 0; i < *nbVariable; i++) {
		struct headerBool* pHeader = (struct headerBool*)p;
		switch (pHeader->type) {
		case 0: {
			set(pHeader->name, pHeader->b, GameEngine::Variable::Type::BOOL);
			p += (sizeof(headerBool) + pHeader->name_len);
			break; }

		case 1: {
			struct headerInt32* pHeader = (struct headerInt32*)p;
			set(pHeader->name, pHeader->i, GameEngine::Variable::Type::INT32);
			p += (sizeof(headerInt32) + pHeader->name_len);
			break; }

		case 2: {
			struct headerFloat* pHeader = (struct headerFloat*)p;
			set(pHeader->name, pHeader->f, GameEngine::Variable::Type::FLOAT);
			p += (sizeof(headerFloat) + pHeader->name_len);
			break;	}

		case 3: {
			struct headerString* pHeader = (struct headerString*)p;
			//set(pHeader->name, pHeader->str_len, GameEngine::Variable::Type::STRING);
			p += (sizeof(headerString) + pHeader->name_len + pHeader->str_len);
			break;
		}

		case 4: {
			struct headerVec3* pHeader = (struct headerVec3*)p;
			set(pHeader->name, pHeader->v3, GameEngine::Variable::Type::VEC3);
			p += (sizeof(headerVec3) + pHeader->name_len);
			break; }

		default:
			break;
		}
	}

	return p - (char*)record;
}
