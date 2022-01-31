#include "dfEnergyClip.h"

#include <imgui.h>

DarkForces::EnergyClip::EnergyClip(void):
	DarkForces::Item("EnergyClip", 0)
{
}

#ifdef _DEBUG
void DarkForces::EnergyClip::debugGUIinline(void)
{
	ImGui::Text("Energy:%d", m_energy);
	ImGui::Text("Max Energy:%d", m_maxEnergy);
}
#endif