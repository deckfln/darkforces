#include "gaPlugin.h"

#include "World.h"

GameEngine::Plugin::Plugin(const char* name):
	m_plugin(name)
{
	g_gaWorld.registerPlugin(this);
}

GameEngine::Plugin::~Plugin()
{
	g_gaWorld.deregisterPlugin(this);
}