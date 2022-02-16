#pragma once

#include "../../gaEngine/gaComponent/gaComponentMesh.h"
#include "../dfObject.h"
#include "../dfComponent/dfCAnim3D.h"

class fwUniform;

namespace DarkForces {
	namespace Anim {
		class ThreeD : public Object {
		protected:
			GameEngine::ComponentMesh m_componentMesh;
			fwUniform* m_uniformAmbient = nullptr;
			DarkForces::Component::Anim3D m_anim;

			void init(const std::string& model);

			void onMove(gaMessage* message);

		public:
			ThreeD(const std::string& model, const glm::vec3& p, float ambient);
			ThreeD(const std::string& model, const glm::vec3& p, float ambient, uint32_t objectID);
			void dispatchMessage(gaMessage* message) override;
		};
	}
}