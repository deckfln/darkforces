#include "gaValue.h"

#include "gaComponent/gaBehaviorTree.h"

//-------------------------------------

static const std::map<const char*, GameEngine::Value::Type> g_types1 = {
	{"bool", GameEngine::Value::Type::BOOL},
	{"int32", GameEngine::Value::Type::INT32},
	{"float", GameEngine::Value::Type::FLOAT},
	{"vec3", GameEngine::Value::Type::VEC3},
	{"string", GameEngine::Value::Type::STRING},
	{"var", GameEngine::Value::Type::VAR},
	{"ptr", GameEngine::Value::Type::PTR},
};


/**
 * initialize variable from XML
 */
void GameEngine::Value::set(tinyxml2::XMLElement* xmlVar)
{
	const char* type = xmlVar->Attribute("type");
	for (auto& t : g_types1) {
		if (strcmp(type, t.first) == 0) {
			m_type = t.second;
			break;
		}
	}

#ifdef _DEBUG
	if (m_type == Type::NONE) {
		gaDebugLog(1, "GameEngine::Value", "unknown type " + (std::string)type);
		exit(-1);
	}
#endif

	const char* v = xmlVar->GetText();

	if (v) {
		if (v[0] == '=') {
			m_type = GameEngine::Value::Type::VAR;
			m_svalue = v + 1;
		}
		else {
			switch (m_type) {
			case Type::BOOL:
				m_value = strcmp(v, "true") == 0;
				break;
			case Type::INT32:
				m_ivalue = std::stoi(v);
				break;
			case Type::FLOAT:
				m_fvalue = std::stof(v);
				break;
			case Type::VEC3: {
				float x, y, z;
				if (v != nullptr) {
					sscanf_s(v, "%f,%f,%f", &x, &y, &z);
					m_v3value = glm::vec3(x, y, z);
				}
				// value defined by code, so not available here
				break; }
			case Type::STRING:
				m_svalue = v;
				break;
			}
		}
	}
}

/**
 *
 */
bool GameEngine::Value::create(tinyxml2::XMLElement* xml)
{
	tinyxml2::XMLElement* xmlValue = xml->FirstChildElement("value");
	if (xmlValue != nullptr) {
		set(xmlValue);
		return true;
	}

	return false;
}

/**
 *
 */
void GameEngine::Value::get(bool& b, GameEngine::Component::BehaviorTree* tree) {
	if (m_type == Type::BOOL) {
		b = m_value;
	}
	else {
		b = tree->blackboard().get<bool>(m_svalue, GameEngine::Variable::Type::BOOL);
	}
}

/**
 *
 */
void GameEngine::Value::get(int32_t& b, GameEngine::Component::BehaviorTree* tree) {
	if (m_type == Type::INT32) {
		b = m_ivalue;
	}
	else {
		b = tree->blackboard().get<int32_t>(m_svalue, GameEngine::Variable::Type::INT32);
	}
}

/**
 *
 */
void GameEngine::Value::get(float& b, GameEngine::Component::BehaviorTree* tree) {
	if (m_type == Type::FLOAT) {
		b = m_fvalue;
	}
	else {
		b = tree->blackboard().get<float>(m_svalue, GameEngine::Variable::Type::FLOAT);
	}
}

/**
 *
 */
void GameEngine::Value::get(glm::vec3& b, GameEngine::Component::BehaviorTree* tree) {
	if (m_type == Type::VEC3) {
		b = m_v3value;
	}
	else {
		b = tree->blackboard().get<glm::vec3>(m_svalue, GameEngine::Variable::Type::VEC3);
	}
}

/**
 *
 */
bool& GameEngine::Value::getb(GameEngine::Component::BehaviorTree* tree)  {
	if (m_type == Type::BOOL) {
		return m_value;
	}
	else {
		return tree->blackboard().get<bool>(m_svalue, GameEngine::Variable::Type::BOOL);
	}
}

/**
 *
 */
int32_t& GameEngine::Value::geti(GameEngine::Component::BehaviorTree* tree)  {
	if (m_type == Type::INT32) {
		return m_ivalue;
	}
	else {
		return tree->blackboard().get<int32_t>(m_svalue, GameEngine::Variable::Type::INT32);
	}
}

/**
 *
 */
float& GameEngine::Value::getf(GameEngine::Component::BehaviorTree* tree) 
{
	if (m_type == Type::FLOAT) {
		return m_fvalue;
	}
	else {
		return tree->blackboard().get<float>(m_svalue, GameEngine::Variable::Type::FLOAT);
	}
}

/**
 *
 */
glm::vec3& GameEngine::Value::getv3(GameEngine::Component::BehaviorTree* tree) {
	if (m_type == Type::VEC3) {
		return m_v3value;
	}
	else {
		return tree->blackboard().get<glm::vec3>(m_svalue, GameEngine::Variable::Type::VEC3);
	}
}

/**
 *
 */
std::string& GameEngine::Value::gets(GameEngine::Component::BehaviorTree* tree)  {
	if (m_type == Type::STRING) {
		return m_svalue;
	}
	else {
		return tree->blackboard().get<std::string>(m_svalue, GameEngine::Variable::Type::STRING);
	}
}

//----------------------------------------------------

#ifdef _DEBUG
/**
 *
 */
const char* GameEngine::Value::debug(void)
{
	static char tmp[64];

	switch (m_type) {
	case Type::BOOL:
		snprintf(tmp, 64, "%db", m_value);
		break;
	case Type::INT32:
		snprintf(tmp, 64, "%d", m_ivalue);
		break;
	case Type::FLOAT:
		snprintf(tmp, 64, "%.2f", m_fvalue);
		break;
	case Type::VEC3: {
		snprintf(tmp, 64, "%.2f %.2f %.2f", m_v3value.x, m_v3value.y, m_v3value.z);
		break; }
	case Type::VAR:
	case Type::STRING:
		snprintf(tmp, 64, "%s", m_svalue.c_str());
		break;
	}

	return tmp;
}
#endif