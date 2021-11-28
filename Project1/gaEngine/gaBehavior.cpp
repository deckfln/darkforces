#include "gaBehavior.h"

#include <map>
#include "gaBehaviorNode/gaBehaviorDecorator.h"
#include "gaBehaviorNode/gaBehaviorLoop.h"
#include "gaBehaviorNode/gaBehaviorSequence.h"
#include "gaBehaviorNode/gaBehaviorSound.h"
#include "gaBehaviorNode/gaBNSatNav.h"
#include "gaBehaviorNode/gaMoveTo.h"

static std::map<std::string, GameEngine::Behavior::createFunction> g_createNodes = {
	{"Decorator", GameEngine::Behavior::Decorator::create},
	{"Loop", GameEngine::Behavior::Loop::create},
	{"Sequence", GameEngine::Behavior::Sequence::create},
	{"Sound", GameEngine::Behavior::Sound::create},
	{"SatNav", GameEngine::Behavior::SatNav::create},
	{"MoveTo", GameEngine::Behavior::MoveTo::create}
};

void GameEngine::Behavior::registerNode(const std::string& name, createFunction create)
{
	g_createNodes[name] = create;
}

GameEngine::Component::BehaviorTree* GameEngine::Behavior::loadTree(const std::string& data)
{
	return nullptr;
}
 