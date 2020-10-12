#include "Model.h"

using namespace GameEngine;

uint32_t g_modelIDs = 0;
uint32_t GameEngine::Model::m_modelClasses = 32;

Model::Model(const std::string& name, uint32_t myclass):
	m_modelID(g_modelIDs++),
	m_name(name),
	m_modelClass(myclass)
{
}

bool Model::named(const std::string& name)
{
	return m_name == name;
}

Model::Model(uint32_t myclass):
	m_modelID(g_modelIDs++),
	m_modelClass(myclass)
{
}
