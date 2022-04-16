#include "gaComponent.h"
#include "gaEntity.h"

#include <imgui.h>

static uint32_t g_ids=0;

gaComponent::gaComponent(int type, const std::string& name) :
	m_type(type),
	m_id(g_ids++),
	m_name(name)
{
}

inline uint32_t gaComponent::entityID(void)
{
	return m_entity->entityID();
}

/**
 * save the component state in a record
 */
uint32_t gaComponent::recordState(void* record)
{
	// even if a subclass doesn't provide a function, record an component of size 0
	((uint32_t*)record)[0] = sizeof(uint32_t);
	return sizeof(uint32_t);
}

#ifdef _DEBUG
/**
 *  Add dedicated component debug the entity
 */
void gaComponent::debugGUIinline(void)
{
	if (ImGui::TreeNode(m_name.c_str())) {
		ImGui::TreePop();
	}
}
#endif

gaComponent::~gaComponent()
{
}