#include "dfEnergyClip.h"

#include <imgui.h>
#include "../../dfObject.h"

DarkForces::EnergyClip::EnergyClip(void):
	DarkForces::Item("EnergyClip", dfLogic::ITEM_ENERGY, false)
{
}

#ifdef _DEBUG
void DarkForces::EnergyClip::debugGUIinline(void)
{
	ImGui::Text("Energy:%d", m_energy);
	ImGui::Text("Max Energy:%d", m_maxEnergy);
}
#endif