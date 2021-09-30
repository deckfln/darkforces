#include "dfGotoTrigger.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaNavMesh.h"

#include "../dfSector.h"
#include "../dfLogicTrigger.h"
#include "../dfComponent/Trigger.h"
#include "../dfComponent/InfElevator.h"

DarkForces::Behavior::GotoTrigger::GotoTrigger(const char *name):
	GameEngine::BehaviorNode(name)
{
	m_sequence = true;
}

void DarkForces::Behavior::GotoTrigger::init(void *data)
{
	// we are on a natural move, to the elevator can be activated
	// test all triggers of the object

	Component::InfElevator* elevator = static_cast<Component::InfElevator*>(data);

	if (elevator == nullptr) {
		m_status = Status::FAILED;
		return;
	}

	// convert the Trigger elevator to the parent entity
	const std::vector<Component::Trigger*>& cTriggers = elevator->getTriggers();

	for (auto cTrigger : cTriggers) {
		m_triggers.push_back(cTrigger->entity());
	}

	m_status = Status::RUNNING;
	m_next = 0;
}

/**
 * activate the current targeted trigger
 */
void DarkForces::Behavior::GotoTrigger::activate_trigger(void)
{
	// activate the trigger
	m_entity->sendMessage(m_targetTrigger->name(), DF_MESSAGE_TRIGGER);

	// broadcast the end of the move (for animation)
	m_entity->sendMessage(gaMessage::END_MOVE);

	m_triggers.clear();
	m_targetTrigger = nullptr;
	m_status = Status::SUCCESSED;
}

/**
 * Set the destination to the next trigger that can activate an elevator
 */
void DarkForces::Behavior::GotoTrigger::goto_next_trigger(void)
{
	if (m_next < (int32_t)m_triggers.size()) {
		m_targetTrigger = m_triggers[m_next];
		m_next++;
	}
	else {
		// so we tried all triggers and still cant open the door => give up
		m_status = Status::FAILED;
		m_targetTrigger = nullptr;
	}
}

/**
 * move to the next trigger
 */
void DarkForces::Behavior::GotoTrigger::nextNode(Action *r)
{
	if (m_status == Status::WAIT) {
		r->action = sAction::RUNNING;
		return;
	}

	if (m_status == Status::FAILED) {
		r->action = sAction::EXIT;
		r->status = m_status;
		return;
	}

	struct GameEngine::Physics::CollisionList* collidedList = static_cast<struct GameEngine::Physics::CollisionList*>(m_tree->blackboard("lastCollision"));

	if (collidedList != nullptr && collidedList->size == 0) {
		r->action = sAction::EXIT;
		r->status = Status::FAILED;
		return;
	}

	// first execution
	if (m_targetTrigger == nullptr) {
		goto_next_trigger();

		// and go to the first trigger
		//m_entity->sendMessage(gaMessage::Action::SatNav_GOTO, m_targetTrigger->position());
		glm::vec3 p = m_targetTrigger->position();
		r->action = sAction::START_CHILD;
		r->child = 0;
		r->data = &p;
		return;
	}

	// check if we are colliding with the destination
	if (collidedList != nullptr) {
		gaEntity* entity;
		for (auto i = 0; i < collidedList->size; i++) {
			entity = collidedList->entities[i];

			if (m_targetTrigger == entity) {
				activate_trigger();
				r->action = sAction::EXIT;
				r->status = Status::SUCCESSED;
				return;
			}
		}
	}

	// we hit "something" let's test the distance from here to the trigger. if we are near, let's pretend everything is OK
	// and do it in 2D, the trigger can be upward
	glm::vec2 e(m_entity->position().x, m_entity->position().z);
	glm::vec2 t(m_targetTrigger->position().x, m_targetTrigger->position().z);
	float d = glm::distance(e, t);

	if (d < 0.5f) {
		activate_trigger();
		r->action = sAction::EXIT;
		r->status = Status::SUCCESSED;
		return;
	}
	else {
		goto_next_trigger();
		if (m_status == Status::RUNNING) {
			// and go to the first trigger
			//m_entity->sendMessage(gaMessage::Action::SatNav_GOTO, m_targetTrigger->position());
			glm::vec3 p = m_targetTrigger->position();
			r->action = sAction::START_CHILD;
			r->child = 0;
			r->data = &p;
			return;
		}
		else {
			r->action = sAction::EXIT;
			r->status = Status::FAILED;
			return;
		}
	}

	return BehaviorNode::nextNode(r);
}

/**
 * flight recorder status
 */
uint32_t DarkForces::Behavior::GotoTrigger::recordState(void* record)
{
	FlightRecorder::GotoTrigger* r = static_cast<FlightRecorder::GotoTrigger*>(record);

	BehaviorNode::recordState(record);

	uint32_t len = sizeof(FlightRecorder::GotoTrigger);

	r->nbTriggers = m_triggers.size();
	r->next = m_next;

	char* p = &r->triggers[0];
	uint32_t i = 0;
	uint32_t l;

	for (auto trigger : m_triggers) {
		if (trigger == m_targetTrigger) {
			r->target = i;
		}

		l = trigger->name().size() + 1;
		strcpy_s(p, 1024, trigger->name().c_str());

		p += l;
		len += l;

		i++;
	}

	r->node.size = len;
	return len;
}

uint32_t DarkForces::Behavior::GotoTrigger::loadState(void* record)
{
	BehaviorNode::loadState(record);

	FlightRecorder::GotoTrigger* r = static_cast<FlightRecorder::GotoTrigger*>(record);
	char* p = &r->triggers[0];
	gaEntity* entity;

	m_next = r->next;
	m_targetTrigger = nullptr;
	m_triggers.resize(r->nbTriggers);

	for (uint32_t i = 0; i < r->nbTriggers; i++) {
		entity = g_gaWorld.getEntity(p);

		if (i == r->target) {
			m_targetTrigger = entity;
		}
		m_triggers[i] = entity;

		p += strlen(p) + 1;
	}

	return r->node.size;
}
