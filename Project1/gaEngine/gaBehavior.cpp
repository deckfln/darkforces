#include "gaBehavior.h"

#include <map>
#include <tinyxml2.h>

#include "gaBehaviorNode/gaBehaviorDecorator.h"
#include "gaBehaviorNode/gaBehaviorLoop.h"
#include "gaBehaviorNode/gaBehaviorSequence.h"
#include "gaBehaviorNode/gaBehaviorSound.h"
#include "gaBehaviorNode/gaBNSatNav.h"
#include "gaBehaviorNode/gaMoveTo.h"

struct char_cmp {
	bool operator () (const char* a, const char* b) const
	{
		return strcmp(a, b) < 0;
	}
};

static std::map<const char*, GameEngine::Behavior::createFunction, char_cmp> g_createNodes = {
	{"Decorator", GameEngine::Behavior::Decorator::create},
	{"Loop", GameEngine::Behavior::Loop::create},
	{"Sequence", GameEngine::Behavior::Sequence::create},
	{"Sound", GameEngine::Behavior::Sound::create},
	{"SatNav", GameEngine::Behavior::SatNav::create},
	{"MoveTo", GameEngine::Behavior::MoveTo::create}
};

void GameEngine::Behavior::registerNode(const char* name, createFunction create)
{
	g_createNodes[name] = create;
}

static GameEngine::BehaviorNode* loadNode(tinyxml2::XMLElement* node)
{
	GameEngine::BehaviorNode* bnode = nullptr;
	GameEngine::BehaviorNode* bchild = nullptr;

	// get the attributes
	const char* type = node->Attribute("type");
	const char* name = node->Attribute("name");

	if (g_createNodes[type] == nullptr) {
		printf("GameEngine::Behavior::loadTree: unknwon type %s in file", type);
		return nullptr;
	}

	// and create the node
	bnode = (*g_createNodes[type])(_strdup(name), node);

	// and create an bind the children
	tinyxml2::XMLElement* tree = node->FirstChildElement("tree");
	if (tree) {
		tinyxml2::XMLElement* pNodeElement = tree->FirstChildElement("node");

		while (pNodeElement != nullptr) {
			bchild = loadNode(pNodeElement);
			bnode->addNode(bchild);

			pNodeElement = pNodeElement->NextSiblingElement("node");
		}
	}

	return bnode;
}

GameEngine::BehaviorNode* GameEngine::Behavior::loadTree(const std::string& data)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(data.c_str());

	// create the node
	tinyxml2::XMLElement* pRoot = doc.FirstChildElement("node");

	return loadNode(pRoot);
}
 