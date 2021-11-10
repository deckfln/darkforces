#pragma once

#include "../../gaEngine/gaComponent/gaComponentMesh.h"
#include "../dfObject.h"
#include "../dfComponent/dfCAnim3D.h"

namespace DarkForces {
	namespace Anim {
		class ThreeD : public Object {
			GameEngine::ComponentMesh m_componentMesh;
			DarkForces::Component::Anim3D m_anim;

			void init(const std::string& model);

		public:
			ThreeD(const std::string& model, const glm::vec3& p, float ambient);
			ThreeD(const std::string& model, const glm::vec3& p, float ambient, uint32_t objectID);
		};
	}
}