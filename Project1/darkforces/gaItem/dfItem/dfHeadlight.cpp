#include "dfHeadlight.h"

#include "../../dfObject.h"

DarkForces::Headlight::Headlight(const std::string& name) : 
	DarkForces::Item(name, dfLogic::NONE)
{
	m_droppable = false;
}
