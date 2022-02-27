#include "gaBVar.h"

#include <tinyxml2.h>
#include <imgui.h>
#include <map>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"
#include "../gaValue.h"

static const char* g_className = "Var";

//-------------------------------------

GameEngine::Behavior::Var::Var(const char* name) :
	BehaviorNode(name)
{
	m_className = g_className;
}

GameEngine::BehaviorNode* GameEngine::Behavior::Var::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Var* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Var*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Var(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	cl->m_variable = m_variable;
	cl->m_value = m_value;
	return cl;
}

/**
 *
 */
GameEngine::BehaviorNode* GameEngine::Behavior::Var::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Var* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Var(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Var*>(used);
	}
	GameEngine::BehaviorNode::create(name, element, node);

	// get the variable name
	if (!node->m_variable.create(element)) {
		gaDebugLog(1, "GameEngine::Behavior::Var", "no variable defined for " + (std::string)node->m_name);
		exit(-1);
	}
	if (!node->m_value.create(element)) {
		gaDebugLog(1, "GameEngine::Behavior::Var", "no value defined for " + (std::string)node->m_name);
		exit(-1);
	}

	return node;
}

//----------------------------------------------

void GameEngine::Behavior::Var::debugGUInode(GameEngine::Component::BehaviorTree* tree)
{
	std::string v = std::string(m_variable.name()) + "=" + std::string(m_variable.value(tree));
	ImGui::Text(v.c_str());
}
