#include "gaComponent.h"
#include "gaEntity.h"

static uint32_t g_ids=0;

gaComponent::gaComponent(int type) :
	m_type(type),
	m_id(g_ids++)
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

gaComponent::~gaComponent()
{
}