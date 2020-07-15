#include "gaModel.h"

using namespace GameEngine;

uint32_t g_modelIDs = 0;
uint32_t GameEngine::gaModel::m_modelClasses = 0;

gaModel::gaModel(const std::string& name, uint32_t myclass):
	m_modelID(g_modelIDs++),
	m_name(name),
	m_modelClass(myclass)
{
}

bool gaModel::named(const std::string& name)
{
	return m_name == name;
}

gaModel::gaModel(uint32_t myclass):
	m_modelID(g_modelIDs++),
	m_modelClass(myclass)
{
}
