#include "gaVariable.h"

#include "gaComponent/gaBehaviorTree.h"

bool GameEngine::Variable::create(tinyxml2::XMLElement* element)
{
	const std::map<const char*, Type> m_types = {
		{ "bool", Type::BOOL},
		{ "int32", Type::INT32},
		{ "float", Type::FLOAT},
		{ "vec3", Type::VEC3},
		{ "string", Type::STRING},
		{ "ptr", Type::PTR},
	};

	const char* ctype = nullptr;
	const char* cvalue = nullptr;

	tinyxml2::XMLElement* xmlVariable = element->FirstChildElement("variable");
	if (xmlVariable != nullptr) {
		bool found = false;

		m_name = xmlVariable->GetText();
		ctype = xmlVariable->Attribute("type");
		if (ctype) {
			for (auto& m : m_types) {
				if (strcmp(m.first, ctype) == 0) {
					m_type = m.second;
					found = true;
					break;
				}
			}
		}

		if (!found) {
			gaDebugLog(1, "GameEngine::Variable::create", "unknown type " + (std::string)ctype);
			exit(-1);
		}

		// assign variable ?
		const char* value = xmlVariable->Attribute("value");;
		if (value != nullptr) {
			if (value[0] == '=') {
				// assign an other variable
				m_var = value +1 ;
			}
			else {
				// assign a value
				switch (m_type) {
				case Type::BOOL:
					xmlVariable->QueryBoolAttribute("value", &m_value);
					break;
				case Type::INT32:
					xmlVariable->QueryIntAttribute("value", &m_ivalue);
					break;
				case Type::FLOAT:
					xmlVariable->QueryFloatAttribute("value", &m_fvalue);
					break;
				case Type::VEC3: {
					const char* v = xmlVariable->Attribute("value");
					float x, y, z;
					if (v != nullptr) {
						sscanf_s(v, "%f,%f,%f", &x, &y, &z);
						m_v3value = glm::vec3(x, y, z);
					}
					// value defined by code, so not available here
					break; }
				case Type::STRING:
					m_svalue = xmlVariable->Attribute("value");
					break;
				}
			}
		}
	}
	return true;
}

/**
 * get a VEC3
 */
glm::vec3& GameEngine::Variable::getv3(GameEngine::Component::BehaviorTree* tree)
{
	if (m_type != GameEngine::Variable::Type::VEC3) {
		gaDebugLog(1, "GameEngine::Variable::getv3", "incompatible type requested for " + m_name);
		exit(-1);
	}
	return tree->blackboard().get<glm::vec3>(m_name, GameEngine::Variable::Type::VEC3);
}

/**
 * get a STRING
 */
std::string& GameEngine::Variable::gets(GameEngine::Component::BehaviorTree* tree)
{
	if (m_type != GameEngine::Variable::Type::STRING) {
		gaDebugLog(1, "GameEngine::Variable::gets", "incompatible type requested for " + m_name);
		exit(-1);
	}
	return tree->blackboard().get<std::string>(m_name, GameEngine::Variable::Type::STRING);
}

/**
 * get a BOOL
 */
bool& GameEngine::Variable::getb(GameEngine::Component::BehaviorTree* tree)
{
	if (m_type != GameEngine::Variable::Type::BOOL) {
		gaDebugLog(1, "GameEngine::Variable::getb", "incompatible type requested for " + m_name);
		exit(-1);
	}
	return tree->blackboard().get<bool>(m_name, GameEngine::Variable::Type::BOOL);
}

/**
 * get a INT32
 */
int32_t& GameEngine::Variable::geti(GameEngine::Component::BehaviorTree* tree)
{
	if (m_type != GameEngine::Variable::Type::INT32) {
		gaDebugLog(1, "GameEngine::Variable::geti", "incompatible type requested for " + m_name);
		exit(-1);
	}
	return tree->blackboard().get<int32_t>(m_name, GameEngine::Variable::Type::FLOAT);
}

/**
 * get a FLOAT
 */
float& GameEngine::Variable::getf(GameEngine::Component::BehaviorTree* tree)
{
	if (m_type != GameEngine::Variable::Type::FLOAT) {
		gaDebugLog(1, "GameEngine::Variable::getf", "incompatible type requested for " + m_name);
		exit(-1);
	}
	return tree->blackboard().get<float>(m_name, GameEngine::Variable::Type::FLOAT);
}

/**
 * get a PTR
 */
void* GameEngine::Variable::getp(GameEngine::Component::BehaviorTree* tree)
{
	if (m_type != GameEngine::Variable::Type::PTR) {
		gaDebugLog(1, "GameEngine::Variable::getp", "incompatible type requested for " + m_name);
		exit(-1);
	}
	return tree->blackboard().pGet<void*>(m_name, GameEngine::Variable::Type::PTR);
}

/**
 * set a PTR
 */
void GameEngine::Variable::set(GameEngine::Component::BehaviorTree* tree, void* ptr)
{
	if (m_type != GameEngine::Variable::Type::PTR) {
		gaDebugLog(1, "GameEngine::Variable::set", "incompatible type requested for " + m_name);
		exit(-1);
	}
	tree->blackboard().pSet(m_name, ptr);
}

/**
 * set anINT32
 */
void GameEngine::Variable::set(GameEngine::Component::BehaviorTree* tree, int32_t i)
{
	if (m_type != GameEngine::Variable::Type::INT32) {
		gaDebugLog(1, "GameEngine::Variable::set", "incompatible type requested for " + m_name);
		exit(-1);
	}
	tree->blackboard().set<int32_t>(m_name, i, GameEngine::Variable::Type::INT32);
}

/**
 * set a float
 */
void GameEngine::Variable::set(GameEngine::Component::BehaviorTree* tree, float f)
{
	if (m_type != GameEngine::Variable::Type::FLOAT) {
		gaDebugLog(1, "GameEngine::Variable::set", "incompatible type requested for " + m_name);
		exit(-1);
	}
	tree->blackboard().set<float>(m_name, f, GameEngine::Variable::Type::FLOAT);
}

/**
 * set on the blackboard from an VEC3
 */
void GameEngine::Variable::set(GameEngine::Component::BehaviorTree* tree, const glm::vec3& v)
{
	if (m_type != GameEngine::Variable::Type::VEC3) {
		gaDebugLog(1, "GameEngine::Variable::set", "incompatible type requested for " + m_name);
		exit(-1);
	}
	tree->blackboard().set<glm::vec3>(m_name, v, GameEngine::Variable::Type::VEC3);
}

/**
 * set on the blackboard from an STRING
 */
void GameEngine::Variable::set(GameEngine::Component::BehaviorTree* tree, const std::string& s)
{
	if (m_type != GameEngine::Variable::Type::STRING) {
		gaDebugLog(1, "GameEngine::Variable::set", "incompatible type requested for " + m_name);
		exit(-1);
	}
	tree->blackboard().set<std::string>(m_name, s, GameEngine::Variable::Type::STRING);
}

/**
 * set on the blackboard from a value
 */
void GameEngine::Variable::set(GameEngine::Component::BehaviorTree* tree, Value& v)
{
	/*
	if (m_type != v.m_type) {
		gaDebugLog(1, "GameEngine::Variable::set", "incompatible type requested for " + m_name);
		exit(-1);
	}
	*/

	switch (m_type) {
	case Type::BOOL: {
		bool& b = v.getb(tree);
		set(tree, b);
		break;
	}
	case Type::INT32: {
		int32_t& i = v.geti(tree);
		set(tree, i);
		break;
	}
	case Type::FLOAT: {
		float& f = v.getf(tree);
		set(tree, f);
		break;
	}
	case Type::VEC3: {
		glm::vec3& v3 = v.getv3(tree);
		set(tree, v3);
		break;
	}
	case Type::STRING: {
		std::string& s = v.gets(tree);
		set(tree, s);
		break;
	}
	}
}

/**
 * get the predefined value from the blackboard
 */
void GameEngine::Variable::get(GameEngine::Component::BehaviorTree* tree, const std::string& source)
{
	switch (m_type) {
	case Type::BOOL:
		m_value = tree->blackboard().get<bool>(source, GameEngine::Variable::Type::BOOL);
		break;
	case Type::INT32:
		m_ivalue = tree->blackboard().get<int32_t>(source, GameEngine::Variable::Type::INT32);
		break;
	case Type::FLOAT:
		m_fvalue = tree->blackboard().get<float>(source, GameEngine::Variable::Type::FLOAT);
		break;
	case Type::VEC3:
		m_v3value = tree->blackboard().get<glm::vec3>(source, GameEngine::Variable::Type::VEC3);
		break;
	case Type::STRING:
		m_svalue = tree->blackboard().get<std::string>(source, GameEngine::Variable::Type::STRING);
		break;
	}
}


/**
 * set the predefined content on the blackboard
 */
void GameEngine::Variable::set(GameEngine::Component::BehaviorTree* tree)
{
	if (m_var != "") {
		// assign from another variable
		get(tree, m_var);
	}

	switch (m_type) {
	case Type::BOOL:
		tree->blackboard().set<bool>(m_name, m_value, GameEngine::Variable::Type::BOOL);
		break;
	case Type::INT32:
		tree->blackboard().set<int32_t>(m_name, m_ivalue, GameEngine::Variable::Type::INT32);
		break;
	case Type::FLOAT:
		tree->blackboard().set<float>(m_name, m_fvalue, GameEngine::Variable::Type::FLOAT);
		break;
	case Type::VEC3:
		tree->blackboard().set<glm::vec3>(m_name, m_v3value, GameEngine::Variable::Type::VEC3);
		break;
	case Type::STRING:
		tree->blackboard().set<std::string>(m_name, m_svalue, GameEngine::Variable::Type::STRING);
		break;
	}
}

/**
 * compare the predef value to the blackbord
 */
bool GameEngine::Variable::equal(GameEngine::Component::BehaviorTree* tree, GameEngine::Value& v)
{
	switch (m_type) {
	case Type::BOOL: {
		bool& b = v.getb(tree);
		bool& v1 = tree->blackboard().get<bool>(m_name, GameEngine::Variable::Type::BOOL);
		return (v1 == b);
	}
	case Type::INT32: {
		int32_t& i = v.geti(tree);
		int32_t& i1 = tree->blackboard().get<int32_t>(m_name, GameEngine::Variable::Type::INT32);
		return (i1 == i);
	}
	case Type::FLOAT: {
		float& f = v.getf(tree);
		float& f1 = tree->blackboard().get<float>(m_name, GameEngine::Variable::Type::FLOAT);
		return (f1 == f);
	}
	case Type::VEC3: {
		glm::vec3& v3 = v.getv3(tree);
		glm::vec3& v3a = tree->blackboard().get<glm::vec3>(m_name, GameEngine::Variable::Type::VEC3);
		return (v3a == v3);
	}
	case Type::STRING: {
		std::string& s = v.gets(tree);
		std::string& s1 = tree->blackboard().get<std::string>(m_name, GameEngine::Variable::Type::STRING);
		return (s1 == s);
	}
	}

	return false;
}

//---------------------------------------------------

/**
 *
 */
const char* GameEngine::Variable::debug(void)
{
	static char tmp[64];

	snprintf(tmp, 64, "(%d)%s", m_type, m_name.c_str());

	return tmp;
}
