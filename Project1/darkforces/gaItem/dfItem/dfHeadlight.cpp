#include "dfHeadlight.h"

DarkForces::Headlight::Headlight(const std::string& name) : 
	DarkForces::Item(name, 0)
{
	m_droppable = false;
}
