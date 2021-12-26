#include "gaBehavior.h"

#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include <tinyxml2.h>

#include "gaDebug.h"
#include "gaBehaviorNode.h"

struct char_cmp {
	bool operator () (const char* a, const char* b) const
	{
		return strcmp(a, b) < 0;
	}
};

static std::map<const char*, GameEngine::Behavior::createFunction, char_cmp> g_createNodes;

void GameEngine::Behavior::registerNode(const char* name, createFunction create)
{
	g_createNodes[name] = create;
}

/**
 *
 */
static std::map<const std::string, uint32_t> g_createMessage;
static std::map<const std::string, GameEngine::Component::BehaviorTree::msgHandler> g_createHandler;

void GameEngine::Behavior::registerMessage(const char* name, uint32_t id)
{
	g_createMessage[name] = id;
}

void GameEngine::Behavior::registerHandler(const char* name, GameEngine::Component::BehaviorTree::msgHandler handler)
{
	g_createHandler[name] = handler;
}

/**
 * load static plugins
 */
static GameEngine::BehaviorNode* loadPlugins(tinyxml2::XMLElement* bt, GameEngine::Component::BehaviorTree* tree)
{
	GameEngine::BehaviorNode* bnode = nullptr;
	GameEngine::BehaviorNode* bchild = nullptr;

	// and create an bind the children
	tinyxml2::XMLElement* messages = bt->FirstChildElement("messages");
	if (messages) {
		tinyxml2::XMLElement* pNodeElement = messages->FirstChildElement("message");

		while (pNodeElement != nullptr) {
			const char* id = pNodeElement->Attribute("id");
			const char* handler = pNodeElement->GetText();

			tree->handlers(g_createMessage[id], g_createHandler[handler]);

			pNodeElement = pNodeElement->NextSiblingElement("message");
		}
	}

	return bnode;
}

/**
 * 
 */
static GameEngine::BehaviorNode* loadNode(tinyxml2::XMLElement* node)
{
	GameEngine::BehaviorNode* bnode = nullptr;
	GameEngine::BehaviorNode* bchild = nullptr;

	// get the attributes
	const char* type = node->Attribute("type");
	const char* name = node->Attribute("name");

	if (g_createNodes[type] == nullptr) {
		gaDebugLog(1, "GameEngine::Behavior::loadTree", "unknwon type " + std::string(type) + " in file");
		return nullptr;
	}

	// and create the node
	bnode = (*g_createNodes[type])(_strdup(name), node, nullptr);

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

/**
 *
 */
GameEngine::BehaviorNode* GameEngine::Behavior::loadTree(const std::string& data,
	const std::map<std::string, std::string>& includes,
	GameEngine::Component::BehaviorTree* tree)
{
	std::string xml;
	if (data.substr(0, 5) == "file:") {
		std::ifstream file;

		// ensure ifstream objects can throw exceptions:
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			file.open(data.substr(5));
			std::stringstream vStream;
			// read file's buffer contents into streams
			vStream << file.rdbuf();
			// close file handlers
			file.close();
			// convert stream into string
			xml = vStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "GameEngine::Behavior::loadTree " << data << " ::FILE_NOT_SUCCESFULLY_READ" << std::endl;
			exit(-1);
		}
	}
	else {
		xml = data;
	}
	std::string xmlInclude;
	int hasInclude;

	for (auto& include : includes) {
		xmlInclude = "<include file=\'" + include.first + "\'/>";
		hasInclude = xml.find(xmlInclude);
		if (hasInclude >= 0) {
			xml.replace(hasInclude, xmlInclude.size(), include.second);
		}
	}

	tinyxml2::XMLDocument doc;
	if (doc.Parse(xml.c_str()) != tinyxml2::XML_SUCCESS) {
		std::cout << "GameEngine::Behavior::loadTree " << xml << std::endl;
		exit(-1);
	};

	// load the plugins
	tinyxml2::XMLElement* bt = doc.FirstChildElement("behaviortree");
	loadPlugins(bt, tree);

	// create the node
	tinyxml2::XMLElement* pRoot = bt->FirstChildElement("node");

	return loadNode(pRoot);
}
 