#pragma once

#include "gaBehaviorLoop.h"
#include "gaBVar.h"
#include "../gaValue.h"

namespace GameEngine {
	namespace Behavior {
		class For : public GameEngine::Behavior::Loop {
			enum class typeVar {
				INT32,
				VAR
			};

			std::string m_variable;									// variable to increment

			Value m_start;
			Value m_end;

			Status m_defaultreturn = Status::SUCCESSED;

		public:				
			For(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			void init(void* data) override;							// init the node before running

		protected:
			bool endLoop(void) override;
		};
	}
}