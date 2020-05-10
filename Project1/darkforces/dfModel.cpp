#include "dfModel.h"

dfModel::dfModel(std::string& name):
	m_name(name)
{
}

bool dfModel::named(std::string& name)
{
	return m_name == name;
}

