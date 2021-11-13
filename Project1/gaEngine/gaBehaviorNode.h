#pragma once

#include <vector>
#include "gaMessage.h"

/*
 * Define a default node in a Behavior Tree
 */

class gaEntity;

namespace GameEngine {
	namespace Component {
		class BehaviorTree;
	};

	class BehaviorNode {
	public:
		enum class Status {
			WAIT,
			RUNNING,
			FAILED,
			SUCCESSED,
			NONE,
			START_CHILD,
			EXECUTE,
			EXIT
		};
		struct Action {
			Status action=Status::NONE;
			uint32_t child=0;
			void* data=nullptr;
			Status status = Status::WAIT;

			inline Action(Status a, uint32_t c, void* d, Status s) { action = a; child = c; data = d; status = s; }
			inline Action(void) {}
		};
		BehaviorNode(const char *name);

		// getter/setter
		inline BehaviorNode* parent(void) { return m_parent; };
		inline bool isSequence(void) { return m_sequence; };
		inline void tree(Component::BehaviorTree* tree) { m_tree = tree; };
		inline const char* name(void) { return m_name; };
		inline uint32_t id(void) { return m_id; };
		inline Status status(void) { return m_status; };
		inline void status(Status s) { m_status = s; };

		void instanciate(gaEntity* entity);

		BehaviorNode* addNode(BehaviorNode*);						// add a sub-node to that node
		uint32_t count(uint32_t id);								// count nodes in a tree a register ID
		BehaviorNode* find(uint32_t id);							// find the node with ID
		void record(std::vector<BehaviorNode*>& nodes);				// record the node in a list

		virtual void activated(void);								// a node get re-activated after a child exits
		virtual void execute(Action* r);							// let a parent take a decision with it's current running child
		virtual void init(void *);									// init the node before running
		virtual void dispatchMessage(gaMessage* message, BehaviorNode::Action* r);

		// debugger
		virtual void debugGUIinline(BehaviorNode* current, float x, float& y);	// display the component in the debugger
		virtual void debugGUInode(void);							// display the component in the debugger

		// flight recorder status
		virtual uint32_t recordState(void* record);					// save the component state in a record
		virtual uint32_t loadState(void* record);					// reload a component state from a record

	protected:
		uint32_t m_id = 0;											// uniq ID of the node in the tree
		const char* m_name = nullptr;
		Status m_status = Status::WAIT;
		BehaviorNode* m_parent = nullptr;
		gaEntity* m_entity = nullptr;								// entity this node belongs to
		Component::BehaviorTree* m_tree = nullptr;					// tree this node belongs to
		bool m_sequence = false;
		void* m_data = nullptr;

		int32_t m_runningChild = -1;								// currently running child (-1 = the current node is running)
		std::vector<BehaviorNode*> m_children;

#if defined _DEBUG
		std::vector<uint32_t> m_childrenID;
		uint32_t m_entryAttr;
		float m_x, m_y;
		bool m_pined = false;
#endif

		void failed(Action* r);
		void succeeded(Action* r);
		void startChild(Action* r,uint32_t child, void* data);

		inline virtual void onChildExit(uint32_t child, Status status) {};
		inline virtual void onChildStart(uint32_t child) {};

		friend GameEngine::Component::BehaviorTree;
	};

	namespace FlightRecorder {
		struct BehaviorNode {
			uint32_t size;
			uint32_t id;
			uint32_t status;
			uint32_t runningChild;
		};
	}
}