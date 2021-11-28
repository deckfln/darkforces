#pragma once

/**
 * Bahavior Engine: Behavior Tree
 */

#include "gaComponent/gaBehaviorTree.h"

namespace GameEngine
{
	namespace Behavior {

		typedef GameEngine::BehaviorNode* (*createFunction)(const char* name);

		void registerNode(const std::string& name, createFunction);
		GameEngine::Component::BehaviorTree* loadTree(const std::string& data);
	}
}
