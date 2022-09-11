#pragma once

namespace tinyxml2 {
	class XMLElement;
};

/**
 * Bahavior Engine: Behavior Tree
 */

#include "gaComponent/gaBehaviorTree.h"
#include "gaBehaviorPlugin.h"

namespace GameEngine
{
	class BehaviorNode;

	namespace Behavior {
		typedef GameEngine::BehaviorNode* (*createFunction)(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);

		void registerNode(const char* name, createFunction);
		void registerMessage(const char* name, uint32_t id);
		int32_t getMessage(const char* message);

		void registerHandler(const char* name, GameEngine::Component::BehaviorTree::msgHandler);
		void registerPlugin(const char* name, GameEngine::Behavior::Plugin::Base::pluginHandler);

		GameEngine::BehaviorNode* loadTree(
			const std::string& data,
			const std::map<std::string, std::string>& includes,
			GameEngine::Component::BehaviorTree* tree);
	}
}
