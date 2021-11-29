#pragma once

namespace tinyxml2 {
	class XMLElement;
};

/**
 * Bahavior Engine: Behavior Tree
 */

#include "gaComponent/gaBehaviorTree.h"

namespace GameEngine
{
	class BehaviorNode;

	namespace Behavior {
		typedef GameEngine::BehaviorNode* (*createFunction)(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);

		void registerNode(const char* name, createFunction);
		GameEngine::BehaviorNode* loadTree(
			const std::string& data,
			const std::map<std::string, std::string>& includes);
	}
}
